#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageToVTKImageFilter.h>
#include <vtkImageViewer.h>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCamera.h>
#include <vtkTexture.h>
#include <vtkPlaneSource.h>
#include <vtkImageStack.h>
#include <vtkImageSlice.h>
#include <vtkImageSliceMapper.h>
#include <vector>
#include <vtkCallbackCommand.h>
#include <vtkImageProperty.h>
#include <itkImageSeriesReader.h>
#include <itkNumericSeriesFileNames.h>
#include <vtkUnsignedShortArray.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkBoxWidget.h>
#include <vtkTransform.h>
#include <vtkExtractVOI.h>
#include <vtkPlanes.h>
#include <vtkExtractGeometry.h>
#include <vtkClipVolume.h>
#include <vtkUnstructuredGridAlgorithm.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPlaneWidget.h>
#include <vtkPlane.h>
#include <vtkBoxClipDataSet.h>
#include <vtkAlgorithm.h>
#include <vtkTrivialProducer.h>

// Define a callback for transforming the clipping plane box
// (From Examples\Tutorial\Cone6

class ClippingPlaneInteractionCallback : public vtkCommand {
public:
	static ClippingPlaneInteractionCallback *New() {
		ClippingPlaneInteractionCallback *cb = new ClippingPlaneInteractionCallback;
		cb->widget = NULL;
		cb->plane = vtkPlane::New();

		return cb;
	}

	void Execute(vtkObject *caller, unsigned long vtkNotUsed(eventId), void* vtkNotUsed(callData)) VTK_OVERRIDE {
		// This method executes when the vtkPlaneWidget is altered, and the
		// plane has probably been altered. Update the mapper to see the change.
		widget = reinterpret_cast<vtkPlaneWidget*>(caller);

		widget->GetPlane(plane);
		mapper->AddClippingPlane(plane);

		mapper->Update();
	}

	vtkPlaneWidget *widget;
	vtkPlane *plane;
	vtkFixedPointVolumeRayCastMapper *mapper;
};

int main(int argc, char **argv) {
	// Data type definitions

	//typedef itk::Image<unsigned short, 2> ImageType;
	typedef itk::Image<double, 3> ImageType;						// vtkFixedPointVolumeRayCastMapper can't support short data
	typedef itk::ImageSeriesReader<ImageType> ReaderType;
	typedef itk::ImageToVTKImageFilter <ImageType> ConnectorType;	// To convert image data from ITK to VTK format.
	typedef itk::NumericSeriesFileNames NameGeneratorType;

	// Define variables
	NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();
	ReaderType::Pointer reader = ReaderType::New();
	ConnectorType::Pointer connector = ConnectorType::New();
	vtkTrivialProducer *connectorPort = vtkTrivialProducer::New();
	vtkVolume *volume = vtkVolume::New();
	vtkFixedPointVolumeRayCastMapper *mapper = vtkFixedPointVolumeRayCastMapper::New();

	// Kinda validate input to support both command line input and make debugger use simpler.
	// TODO: Improve checks on input and remove debugger part later on.
	if (argc >= 4) {
		nameGenerator->SetSeriesFormat(argv[1]);
		nameGenerator->SetStartIndex(std::stoi(argv[2]));
		nameGenerator->SetEndIndex(std::stoi(argv[3]));
	}
	else {
		// TODO: Remove this block once main development is done.
		nameGenerator->SetSeriesFormat("F:\\prog\\ITK\\SkeletonViz\\build\\Debug\\data\\img (%d).tif");
		nameGenerator->SetStartIndex(1);
		nameGenerator->SetEndIndex(113);
	}

	// Generate file paths for the reader
	nameGenerator->SetIncrementIndex(1);
	std::vector<std::string> filePaths = nameGenerator->GetFileNames();

	// Load slice image files into memory with series reader.
	reader->SetFileNames(filePaths);
	reader->Update();

	// Convert image data from ITK to VTK format
	connector->SetInput(reader->GetOutput());

	// Create a dummy producer to provide an output port from the connector, which
	// seems to have no support for an output port
	connectorPort->SetOutput(connector->GetOutput());
	connector->Update();
	connectorPort->Update();

	// Set up VTK renderer and camera
	vtkRenderer *ren = vtkRenderer::New();
	vtkRenderWindow *renWin = vtkRenderWindow::New();
	renWin->AddRenderer(ren);
	vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
	vtkInteractorStyleTrackballCamera *istyle = vtkInteractorStyleTrackballCamera::New();
	iren->SetRenderWindow(renWin);
	iren->SetInteractorStyle(istyle);

	// Create transfer functions for...
	// Color
	vtkColorTransferFunction *colorFun = vtkColorTransferFunction::New();
	colorFun->AddRGBPoint(-3024, 0, 0, 0, 0.5, 0.0);
	colorFun->AddRGBPoint(850, .55, .25, .15, 0.5, .92);
	colorFun->AddRGBPoint(1100, .88, .60, .29, 0.33, 0.45);
	colorFun->AddRGBPoint(1200, 1, .94, .95, 0.5, 0.0);
	colorFun->AddRGBPoint(3071, .83, .66, 1, 0.5, 0.0);

	// Opacity
	vtkPiecewiseFunction *opacityFun = vtkPiecewiseFunction::New();
	opacityFun->AddPoint(400, 0.2, 0.5, 0);
	opacityFun->AddPoint(700, 0.8, 0.5, 0.5);
	opacityFun->AddPoint(800, 0.5, 0.5, 0.5);
	opacityFun->AddPoint(1200, 0.3, 0.5, 1);
	opacityFun->AddPoint(300, 1, 0.5, 1);
	
	// Opacity Gradient
	vtkPiecewiseFunction *gradientFun = vtkPiecewiseFunction::New();
	gradientFun->AddPoint(-3024, 0, 0.5, 0.0);
	gradientFun->AddPoint(100, 0, 0.5, 0.0);
	gradientFun->AddPoint(850, 1, 0.5, 0.5);
	gradientFun->AddPoint(3071, 1, 0.5, 0.5);

	// Set up the VolumeProperty and connect to the Volume
	vtkVolumeProperty *prop = vtkVolumeProperty::New();
	prop->SetIndependentComponents(true);
	prop->SetInterpolationTypeToLinear();
	prop->ShadeOn();
	prop->SetAmbient(0.1);
	prop->SetDiffuse(0.9);
	prop->SetSpecular(0.2);
	prop->SetSpecularPower(10.0);
	prop->SetScalarOpacityUnitDistance(0.8919);
	prop->SetColor(colorFun);
	prop->SetGradientOpacity(gradientFun);
	prop->SetScalarOpacity(opacityFun);
	volume->SetProperty(prop);

	// Set up the Mapper and connect to the Volume
	//mapper->SetInputData(connector->GetOutput());
	mapper->SetInputConnection(connectorPort->GetOutputPort());
	mapper->SetBlendModeToComposite();
	volume->SetMapper(mapper);

	// Set up a clipping plane
	vtkPlaneWidget *clipPlane = vtkPlaneWidget::New();
	clipPlane->SetInteractor(iren);
	clipPlane->SetPlaceFactor(1.25);
	clipPlane->SetProp3D(volume);
	clipPlane->PlaceWidget();
	clipPlane->On();

	// Set a callback for interaction
	ClippingPlaneInteractionCallback *callback = ClippingPlaneInteractionCallback::New();
	callback->widget = clipPlane;
	callback->mapper = mapper;
	clipPlane->AddObserver(vtkCommand::InteractionEvent, callback);
	
	// Render the scene
	ren->SetBackground(0.1, 0.2, 0.4);
	renWin->SetSize(512, 512);
	ren->AddVolume(volume);
	ren->ResetCamera();
	renWin->Render();
	iren->Start();

	// Clean Up
	callback->Delete();
	clipPlane->Delete();
	prop->Delete();
	gradientFun->Delete();
	opacityFun->Delete();
	colorFun->Delete();
	iren->Delete();
	ren->Delete();
	renWin->Delete();
	mapper->Delete();
	volume->Delete();
	connectorPort->Delete();

	return 0;
}

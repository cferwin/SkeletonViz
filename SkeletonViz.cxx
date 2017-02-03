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

// Define a callback for transforming the clipping plane box
// (From Examples\Tutorial\Cone6

class ClippingBoxInteractionCallback : public vtkCommand {
public:
	static ClippingBoxInteractionCallback *New() {
		ClippingBoxInteractionCallback *c = new ClippingBoxInteractionCallback;
		c->planes = vtkPlanes::New();

		return c;
	}

	void Execute(vtkObject *caller, unsigned long vtkNotUsed(eventId), void* vtkNotUsed(callData)) VTK_OVERRIDE {
		// Update the mapper's clipping planes with the new box widget planes.
		widget = reinterpret_cast<vtkBoxWidget*>(caller);

		widget->GetPlanes(planes);
		mapper->RemoveAllClippingPlanes();
		mapper->AddClippingPlane(planes->GetPlane(1));		// Back plane
		//mapper->AddClippingPlane(planes->GetPlane(2));
		//mapper->AddClippingPlane(planes->GetPlane(3));
		mapper->AddClippingPlane(planes->GetPlane(4));		// Bottom plane
		//mapper->AddClippingPlane(planes->GetPlane(5));
		//mapper->AddClippingPlane(planes->GetPlane(6));
		mapper->Update();
	}

	vtkBoxWidget *widget;
	vtkFixedPointVolumeRayCastMapper *mapper;
	vtkPlanes *planes;
};

int main(int argc, char **argv) {
	// ITK definitions for handling the slice image
	//typedef itk::Image<unsigned short, 2> ImageType;
	typedef itk::Image<double, 3> ImageType;
	//typedef itk::ImageFileReader<ImageType> ReaderType;
	typedef itk::ImageSeriesReader<ImageType> ReaderType;
	typedef itk::ImageToVTKImageFilter < ImageType> ConnectorType;
	ReaderType::Pointer reader = ReaderType::New();
	ConnectorType::Pointer connector = ConnectorType::New();

	typedef itk::NumericSeriesFileNames NameGeneratorType;
	NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();

	// Kinda validate input
	if (argc >= 4) {
		nameGenerator->SetSeriesFormat(argv[1]);
		nameGenerator->SetStartIndex(std::stoi(argv[2]));
		nameGenerator->SetEndIndex(std::stoi(argv[3]));
	}
	else {
		nameGenerator->SetSeriesFormat("F:\\prog\\ITK\\SkeletonViz\\build\\Debug\\data\\img (%d).tif");
		nameGenerator->SetStartIndex(1);
		nameGenerator->SetEndIndex(113);
	}

	std::vector<std::string> paths;

	// Create a stack actor
	//vtkImageStack *stackActor = vtkImageStack::New();

	// Generate file paths
	/*
	typedef itk::NumericSeriesFileNames NameGeneratorType;
	NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();
	nameGenerator->SetSeriesFormat(argv[1]);
	nameGenerator->SetStartIndex(std::stoi(argv[2]));
	nameGenerator->SetEndIndex(std::stoi(argv[3]));
	*/
	nameGenerator->SetIncrementIndex(1);
	std::vector<std::string> filePaths = nameGenerator->GetFileNames();

	reader->SetFileNames(filePaths);
	reader->Update();

	// Initialize the volume
	//vtkImageData *vol = vtkImageData::New();
	vtkVolume *volume = vtkVolume::New();
	connector->SetInput(reader->GetOutput());

	// Set up VTK renderer
	vtkRenderer *ren = vtkRenderer::New();
	vtkRenderWindow *renWin = vtkRenderWindow::New();
	renWin->AddRenderer(ren);
	vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
	vtkInteractorStyleTrackballCamera *istyle = vtkInteractorStyleTrackballCamera::New();
	iren->SetRenderWindow(renWin);
	iren->SetInteractorStyle(istyle);

	// Add volume mapper
	vtkFixedPointVolumeRayCastMapper *mapper = vtkFixedPointVolumeRayCastMapper::New();
	connector->Update();
	
	mapper->SetInputData(connector->GetOutput());
	double spacing[3];
	connector->GetOutput()->GetSpacing(spacing);

	// Create the transfer function
	vtkColorTransferFunction *colorFun = vtkColorTransferFunction::New();
	vtkPiecewiseFunction *opacityFun = vtkPiecewiseFunction::New();
	vtkPiecewiseFunction *gradientFun = vtkPiecewiseFunction::New();

	// Create the property and attach transfer functions
	vtkVolumeProperty *prop = vtkVolumeProperty::New();
	prop->SetIndependentComponents(true);
	prop->SetInterpolationTypeToLinear();

	colorFun->AddRGBPoint(-3024, 0, 0, 0, 0.5, 0.0);
	colorFun->AddRGBPoint(850, .55, .25, .15, 0.5, .92);
	colorFun->AddRGBPoint(1100, .88, .60, .29, 0.33, 0.45);
	colorFun->AddRGBPoint(1200, 1, .94, .95, 0.5, 0.0);
	colorFun->AddRGBPoint(3071, .83, .66, 1, 0.5, 0.0);
	
	opacityFun->AddPoint(-3024, 0, 0.5, 0.0);
	opacityFun->AddPoint(550, 0, 0.5, 0.4);
	opacityFun->AddPoint(900, 0.5, 0.5, 0.61);
	opacityFun->AddPoint(1300, 0.9, 0.5, 0.0);
	opacityFun->AddPoint(3071, 0.9, 0.5, 0.0);
	
	gradientFun->AddPoint(-3024, 0, 0.5, 0.0);
	gradientFun->AddPoint(100, 0, 0.5, 0.0);
	gradientFun->AddPoint(850, 1, 0.5, 0.5);
	gradientFun->AddPoint(3071, 1, 0.5, 0.5);

	/*
	vtkPlaneWidget *planeWidget = vtkPlaneWidget::New();
	planeWidget->SetInteractor(iren);
	planeWidget->SetPlaceFactor(1.25);
	planeWidget->SetProp3D(volume);
	planeWidget->PlaceWidget();

	vtkPlane *plane = vtkPlane::New();
	planeWidget->GetPlane(plane);
	*/

    mapper->SetBlendModeToComposite();
	prop->ShadeOn();
	prop->SetAmbient(0.1);
	prop->SetDiffuse(0.9);
	prop->SetSpecular(0.2);
	prop->SetSpecularPower(10.0);
	prop->SetScalarOpacityUnitDistance(0.8919);
	prop->SetColor(colorFun);
	prop->SetGradientOpacity(gradientFun);
	prop->SetScalarOpacity(opacityFun);

	// Connect the volume to property and mapper
	volume->SetProperty(prop);
	volume->SetMapper(mapper);

	// Create a box widget to control the clipping planes
	vtkBoxWidget *boxWidget = vtkBoxWidget::New();
	boxWidget->SetInteractor(iren);
	boxWidget->SetPlaceFactor(0.25);
	boxWidget->SetProp3D(volume);
	boxWidget->PlaceWidget();
	boxWidget->On();

	// Set a callback for interaction
	ClippingBoxInteractionCallback *callback = ClippingBoxInteractionCallback::New();
	callback->widget = boxWidget;
	callback->mapper = mapper;
	boxWidget->GetPlanes(callback->planes);
	boxWidget->AddObserver(vtkCommand::InteractionEvent, callback);

	// Render the scene
	ren->SetBackground(0.1, 0.2, 0.4);
	renWin->SetSize(512, 512);
	ren->AddVolume(volume);
	ren->ResetCamera();
	renWin->Render();
	iren->Start();

	// Clean Up
	prop->Delete();
	gradientFun->Delete();
	opacityFun->Delete();
	colorFun->Delete();
	boxWidget->Delete();
	mapper->Delete();
	volume->Delete();
	iren->Delete();
	ren->Delete();
	renWin->Delete();

	return 0;
}

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

class ClippingBoxInteractionCallback : public vtkCommand {
public:
	static ClippingBoxInteractionCallback *New() {
		ClippingBoxInteractionCallback *c = new ClippingBoxInteractionCallback;
		c->planes = vtkPlanes::New();
		c->plane = vtkPlane::New();
		//c->normals = vtkDataArray::New();

		return c;
	}

	void Execute(vtkObject *caller, unsigned long vtkNotUsed(eventId), void* vtkNotUsed(callData)) VTK_OVERRIDE {
		// Update the mapper's clipping planes with the new box widget planes.
		widget = reinterpret_cast<vtkPlaneWidget*>(caller);

		/*
		// TODO:
		// Flip the normals to make the box bound the visible volume
		widget->GetPlanes(planes);
		normals = planes->GetNormals();
		
		mapper->RemoveAllClippingPlanes();
		mapper->SetClippingPlanes(planes);
		planes->SetNormals(planes->GetNormals());
		*/
		/*
		mapper->AddClippingPlane(planes->GetPlane(1));		// Back plane
		mapper->AddClippingPlane(planes->GetPlane(2));
		mapper->AddClippingPlane(planes->GetPlane(3));
		mapper->AddClippingPlane(planes->GetPlane(4));		// Bottom plane
		//mapper->AddClippingPlane(planes->GetPlane(5));
		//mapper->AddClippingPlane(planes->GetPlane(6));
		*/

		// TEST: BOX CLIP
		
		//widget->GetPlanes(planes);
		/*
		clipper->GenerateClipScalarsOn();
		clipper->SetBoxClip(planes->GetPlane(0)->GetNormal(),
			planes->GetPlane(0)->GetOrigin(),
			planes->GetPlane(1)->GetNormal(),
			planes->GetPlane(1)->GetOrigin(),
			planes->GetPlane(2)->GetNormal(),
			planes->GetPlane(2)->GetOrigin(),
			planes->GetPlane(3)->GetNormal(),
			planes->GetPlane(3)->GetOrigin(),
			planes->GetPlane(4)->GetNormal(),
			planes->GetPlane(4)->GetOrigin(),
			planes->GetPlane(5)->GetNormal(),
			planes->GetPlane(5)->GetOrigin());
		*/

		// TEST: Clip Volume
		/*
		clipper->SetClipFunction(planes);
		clipper->Update();
		*/
		//mapper->SetClippingPlanes(planes);
		// TODO: Planes 0 and 3 are perpendicular to eachother, but both clip as infinite planes
		//		 Add ARGV arguments for plane numbers and test permutations for a good structure,
		//		 or figure out how to define two easily controllable PLANE WIDGETS.
		//mapper->AddClippingPlane(planes->GetPlane(0));
		//mapper->AddClippingPlane(planes->GetPlane(3));

		// TEST: SINGLE CLIP PLANE
		//mapper->RemoveAllClippingPlanes();
		widget->GetPlane(plane);
		mapper->AddClippingPlane(plane);

		mapper->Update();
	}

	//vtkBoxWidget *widget;
	vtkPlaneWidget *widget;
	vtkPlane *plane;
	vtkFixedPointVolumeRayCastMapper *mapper;
	vtkPlanes *planes;
	vtkDataArray *normals;
	vtkClipVolume *clipper;
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

	// Create a dummy producer to provide an output port from the connector, which
	// seems to have no support for an output port
	vtkTrivialProducer *connectorPort = vtkTrivialProducer::New();
	connectorPort->SetOutput(connector->GetOutput());
	connector->Update();
	connectorPort->Update();

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
	
	// Add a clipping box
	mapper->SetInputData(connector->GetOutput());
	vtkClipVolume *clipVolume = vtkClipVolume::New();
	clipVolume->SetInputData(connector->GetOutput());
	clipVolume->SetInputConnection(connectorPort->GetOutputPort());

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
	
	/*
	opacityFun->AddPoint(-3024, 0, 0.5, 0.0);
	opacityFun->AddPoint(550, 0, 0.5, 0.4);
	opacityFun->AddPoint(900, 0.8, 0.5, 0.61);
	opacityFun->AddPoint(1300, 1, 0.5, 0.0);
	opacityFun->AddPoint(3071, 1, 0.5, 0.0);
	*/
	

	/* my color
	colorFun->AddRGBPoint(1200, 1, 0.95, 0.95, 0.5, 1);
	colorFun->AddRGBPoint(900, 0.7, 0.1, 0.15, 0.5, 0.92);
	colorFun->AddRGBPoint(600, 0.1, 0.1, 0.8, 0.5, 0.9);
	colorFun->AddRGBPoint(-3024, 0, 0, 0, 0.5, 0.0);
	*/

	/* TRY 2
	opacityFun->AddPoint(600, 0.2, 0.5, 1);
	opacityFun->AddPoint(900, 0.9, 0.5, 1);
	opacityFun->AddPoint(1200, 1, 0.7, 1);
	*/
	opacityFun->AddPoint(400, 0.2, 0.5, 0);
	opacityFun->AddPoint(700, 0.8, 0.5, 0.5);
	opacityFun->AddPoint(800, 0.5, 0.5, 0.5);
	//opacityFun->AddPoint(1200, 0.5, 0.5, 0.5);
	//opacityFun->AddPoint(3000, 1, 0.5, 0.5);
	opacityFun->AddPoint(1200, 0.3, 0.5, 1);
	opacityFun->AddPoint(300, 1, 0.5, 1);
	
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

	// Connect the volume to property and mapper
	volume->SetProperty(prop);
	volume->SetMapper(mapper);

	// Create a box widget to control the clipping planes
	/*
	vtkBoxWidget *boxWidget = vtkBoxWidget::New();
	boxWidget->SetInteractor(iren);
	boxWidget->SetPlaceFactor(0.25);
	boxWidget->SetProp3D(volume);
	boxWidget->PlaceWidget();
	boxWidget->On();
	*/

	// Set the clipping box to use the box widget
	/*
	vtkPlanes *planes = vtkPlanes::New();
	boxWidget->GetPlanes(planes);
	vtkPlane *p = planes->GetPlane(1);
	*/
	//clipVolume->GenerateClipScalarsOn();
	/*
	boxClip->SetBoxClip(planes->GetPlane(0)->GetNormal(),
	planes->GetPlane(0)->GetOrigin(),
	planes->GetPlane(1)->GetNormal(),
	planes->GetPlane(1)->GetOrigin(),
	planes->GetPlane(2)->GetNormal(),
	planes->GetPlane(2)->GetOrigin(),
	planes->GetPlane(3)->GetNormal(),
	planes->GetPlane(3)->GetOrigin(),
	planes->GetPlane(4)->GetNormal(),
	planes->GetPlane(4)->GetOrigin(),
	planes->GetPlane(5)->GetNormal(),
	planes->GetPlane(5)->GetOrigin());
	*/
	//clipVolume->SetClipFunction(planes);
	// !!!
	//clipVolume->Update();

	// TEST: Set up a clipping plane
	vtkPlaneWidget *clipPlane = vtkPlaneWidget::New();
	clipPlane->SetInteractor(iren);
	clipPlane->SetPlaceFactor(1.25);
	clipPlane->SetProp3D(volume);
	clipPlane->PlaceWidget();
	clipPlane->On();

	// Set a callback for interaction
	ClippingBoxInteractionCallback *callback = ClippingBoxInteractionCallback::New();
	//callback->widget = boxWidget;
	callback->widget = clipPlane;
	callback->mapper = mapper;
	callback->clipper = clipVolume;
	//boxWidget->GetPlanes(callback->planes);
	//boxWidget->AddObserver(vtkCommand::InteractionEvent, callback);
	clipPlane->AddObserver(vtkCommand::InteractionEvent, callback);

	// !!!
	//mapper->SetInputConnection(clipVolume->GetOutputPort());
	mapper->SetInputConnection(connectorPort->GetOutputPort());
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
	//boxWidget->Delete();
	clipPlane->Delete();
	mapper->Delete();
	volume->Delete();
	iren->Delete();
	ren->Delete();
	renWin->Delete();

	return 0;
}

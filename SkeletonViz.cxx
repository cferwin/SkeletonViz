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
#include "SkeletonViz.h"
#include "ClippingPlaneInteractionCallback.h"
#include "ClippingPlaneMaker.h"
#include "ClippingPlaneAddRemoveCallback.h"

int main(int argc, char **argv) {
    // Set the default visualization type
    currentVisualizationType = BONE_ONLY;       // Segmented skeletal structure only
    //currentVisualizationType = BONE_AND_TISSUE; // Bone in the context of partially-visible surrounding tissue

    // Define variables
    NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();
    ReaderType::Pointer reader = ReaderType::New();
    ConnectorType::Pointer connector = ConnectorType::New();
    vtkTrivialProducer *connectorPort = vtkTrivialProducer::New();
    vtkVolume *volume = vtkVolume::New();
    MapperType *mapper = vtkFixedPointVolumeRayCastMapper::New();
    ThresholdFilterType::Pointer thresholdFilter = itk::ThresholdImageFilter<ImageType>::New();
    const int boneLevel = 1200;

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

    // Isolate the areas of interest.
    if (currentVisualizationType == BONE_ONLY) {
        // Perform threshold segmentation using the bone value
        thresholdFilter->SetInput(reader->GetOutput());
        thresholdFilter->ThresholdBelow(boneLevel);
        thresholdFilter->SetOutsideValue(0);
        thresholdFilter->Update();
    }

	// Convert image data from ITK to VTK format
    if (currentVisualizationType == BONE_ONLY) {
        connector->SetInput(thresholdFilter->GetOutput());
    }
    else {
        connector->SetInput(reader->GetOutput());
    }

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
    vtkColorTransferFunction *colorFun = vtkColorTransferFunction::New();
    vtkPiecewiseFunction *opacityFun = vtkPiecewiseFunction::New();
    vtkPiecewiseFunction *gradientFun = vtkPiecewiseFunction::New();

    if (currentVisualizationType == BONE_ONLY) {
        // Color
        colorFun->AddRGBPoint(-3024, 0, 0, 0, 0.5, 0.0);
        colorFun->AddRGBPoint(860, 1, 0.92, 0.54, 0.5, 0.0);
        //colorFun->AddRGBPoint(860, 0.83, 0.94, 1, 0.63, 0.0);
        //colorFun->AddRGBPoint(4096, 1, 1, 1, 1, 0.5);

        // Opacity
        opacityFun->AddPoint(-3024, 0, 0.5, 0.0);
        opacityFun->AddPoint(boneLevel - 1, 0, 0.5, 0.0);
        opacityFun->AddPoint(boneLevel, 1, 0.5, 0.0);
    }
    else {
        // Color
        colorFun->AddRGBPoint(-3024, 0, 0, 0, 0.5, 0.0);
        //colorFun->AddRGBPoint(860, 0.83, 0.94, 1, 0.63, 0.0);
        colorFun->AddRGBPoint(860, 1, 0.92, 0.54, 0.5, 0.5);
        //colorFun->AddRGBPoint(4096, 1, 1, 1, 1, 0.5);

        // Opacity
        opacityFun->AddPoint(-3024, 0, 0.5, 0.0);
        //opacityFun->AddPoint(1000, 1, 0.5, 0.0);
        opacityFun->AddPoint(boneLevel, 1, 0.5, 0.8);
        opacityFun->AddPoint(4096, 1, 0.5, 0.0);

        // Opacity Gradient
        gradientFun->AddPoint(-3024, 0, 0.5, 0.0);
        gradientFun->AddPoint(100, 0, 0.5, 0.0);
        gradientFun->AddPoint(2700, 1, 0.5, 1);
        gradientFun->AddPoint(4096, 1, 0.5, 1);
    }

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
	prop->SetScalarOpacity(opacityFun);
    if (currentVisualizationType == BONE_AND_TISSUE) {
        prop->SetGradientOpacity(gradientFun);
    }
    volume->SetProperty(prop);

	// Set up the Mapper and connect to the Volume
	mapper->SetInputConnection(connectorPort->GetOutputPort());
	mapper->SetBlendModeToComposite();
	volume->SetMapper(mapper);

	// Add a callback for adding/removing clipping planes with the (A)dd / (D)elete keys
	ClippingPlaneAddRemoveCallback *cb = ClippingPlaneAddRemoveCallback::New();
	cb->mapper = mapper;
	cb->prop = volume;
	iren->AddObserver(vtkCommand::KeyPressEvent, cb);

	// Set up a clipping plane
	vtkPlaneWidget *plane = ClippingPlaneMaker::AddClippingPlane(iren, volume, mapper);
	
	// Render the scene
	ren->SetBackground(0.1, 0.2, 0.4);
	renWin->SetSize(512, 512);
	ren->AddVolume(volume);
	ren->ResetCamera();
	renWin->Render();
	iren->Start();

	// Clean Up
	ClippingPlaneMaker::RemoveClippingPlane(plane, mapper);
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

#include "BoneOnlyPipeline.h"

BoneOnlyPipeline::BoneOnlyPipeline(itk::ImageSource<ImageType> *input) {
    std::cout << "STARTING BONE ONLY CONSTRUCTION" << std::endl;
    // Initialize variables (pipeline objects)
    connector = ConnectorType::New();           // To convert from ITK to VTK
    connectorPort = vtkTrivialProducer::New();  //
    volume = vtkVolume::New(); // Volume actor for the image data
    mapper = vtkFixedPointVolumeRayCastMapper::New(); // Mapper for the volume
    thresholdFilter = itk::ThresholdImageFilter<ImageType>::New(); // Threshold segmentation filter
    colorFun = vtkColorTransferFunction::New(); // Transfer functions for color...
    opacityFun = vtkPiecewiseFunction::New();   // opacity...
    gradientFun = vtkPiecewiseFunction::New();  // and gradient opacity.
    prop = vtkVolumeProperty::New(); // Volume property
    addRemoveCallback = ClippingPlaneAddRemoveCallback::New(); // Callback to add or remove clipping planes
    //modeCallback = VisualizationModeCallback::New();
    //modeCallback = cb;
    const int boneLevel = 1200;

    // Isolate the areas of interest.
    if (currentVisualizationType == BONE_ONLY) {
        // Perform threshold segmentation using the bone value
        thresholdFilter->SetInput(input->GetOutput());
        thresholdFilter->ThresholdBelow(boneLevel);
        thresholdFilter->SetOutsideValue(0);
        thresholdFilter->Update();
    }

    // Convert image data from ITK to VTK format
    if (currentVisualizationType == BONE_ONLY) {
        connector->SetInput(thresholdFilter->GetOutput());
    }
    else {
        //connector->SetInput(reader->GetOutput());
    }

    // Create a dummy producer to provide an output port from the connector, which
    // seems to have no support for an output port
    connectorPort->SetOutput(connector->GetOutput());
    connector->Update();
    connectorPort->Update();

    // Set up VTK renderer and camera
    //vtkRenderer *ren = vtkRenderer::New();
    //vtkRenderWindow *renWin = vtkRenderWindow::New();
    //renWin->AddRenderer(ren);
    //vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
    //vtkInteractorStyleTrackballCamera *istyle = vtkInteractorStyleTrackballCamera::New();
    //iren->SetRenderWindow(renWin);
    //iren->SetInteractorStyle(istyle);

    // Create transfer functions for...

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
    addRemoveCallback->mapper = mapper;
    addRemoveCallback->prop = volume;
    //iren->AddObserver(vtkCommand::KeyPressEvent, addRemoveCallback);

    // Add a callback for changing the visualization mode
    //modeCallback->mapper = mapper;
    //iren->AddObserver(vtkCommand::KeyPressEvent, modeCallback);

    // Set up a clipping plane
    //plane = ClippingPlaneMaker::AddClippingPlane(iren, volume, mapper);
    
    std::cout << "DONE WITH BONE ONLY CONSTRUCTION" << std::endl;
}

BoneOnlyPipeline::~BoneOnlyPipeline() {
    std::cout << "STARTING BONE ONLY DECONSTRUCTION" << std::endl;
    connector->Delete();
    connectorPort->Delete();
    volume->Delete();
    mapper->Delete();
    prop->Delete();

    // Transfer functions
    colorFun->Delete();
    opacityFun->Delete();
    gradientFun->Delete();

    // Interaction callbacks
    addRemoveCallback->Delete(); // Add or remove clipping planes
    //modeCallback->Delete(); // Change render mode

    std::cout << "DONE WITH BONE_ONLY DECONSTRUCTION" << std::endl;
}

void BoneOnlyPipeline::addToRenderer(vtkRenderer *ren, vtkRenderWindowInteractor *iren) {
    std::cout << "ADDING BONEONLY TO RENDERER" << std::endl;
    // Set callbacks
    iren->AddObserver(vtkCommand::KeyPressEvent, addRemoveCallback);
    //iren->AddObserver(vtkCommand::KeyPressEvent, modeCallback);
   
    // Add a clipping plane
    plane = ClippingPlaneMaker::AddClippingPlane(iren, volume, mapper);

    // Add the volume to the renderer
    ren->AddVolume(volume);
}

void BoneOnlyPipeline::removeFromRenderer(vtkRenderer *ren, vtkRenderWindowInteractor *iren) {
    std::cout << "REMOVING BONEONLY FROM RENDERER" << std::endl;

    // Remove callbacks
    iren->RemoveObserver(addRemoveCallback);

    // Remove clipping plane
    ClippingPlaneMaker::RemoveClippingPlane(plane, mapper);

    // Remove volume
    ren->RemoveVolume(volume);
}
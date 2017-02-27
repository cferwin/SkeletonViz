#include "BoneAndTissuePipeline.h"

BoneAndTissuePipeline::BoneAndTissuePipeline(itk::ImageSource<ImageType> *input) {
    // Initialize variables (pipeline objects)
    connector = ConnectorType::New();           // To convert from ITK to VTK
    connectorPort = vtkTrivialProducer::New();  //
    volume = vtkVolume::New(); // Volume actor for the image data
    mapper = vtkFixedPointVolumeRayCastMapper::New(); // Mapper for the volume
    colorFun = vtkColorTransferFunction::New(); // Transfer functions for color...
    opacityFun = vtkPiecewiseFunction::New();   // opacity...
    gradientFun = vtkPiecewiseFunction::New();  // and gradient opacity.
    prop = vtkVolumeProperty::New(); // Volume property
    addRemoveCallback = ClippingPlaneAddRemoveCallback::New(); // Callback to add or remove clipping planes

    // Convert image data from ITK to VTK format
    connector->SetInput(input->GetOutput());

    // Create a dummy producer to provide an output port from the connector, which
    // seems to have no support for an output port
    connectorPort->SetOutput(connector->GetOutput());
    connector->Update();
    connectorPort->Update();

    // Create transfer functions for...
    // Color
    colorFun->AddRGBPoint(-3024, 0, 0, 0, 0.5, 0.0);
    //colorFun->AddRGBPoint(860, 0.83, 0.94, 1, 0.63, 0.0);
    colorFun->AddRGBPoint(860, 1, 0.92, 0.54, 0.5, 0.5);
    //colorFun->AddRGBPoint(4096, 1, 1, 1, 1, 0.5);

    // Opacity
    opacityFun->AddPoint(-3024, 0, 0.5, 0.0);
    //opacityFun->AddPoint(1000, 1, 0.5, 0.0);
    opacityFun->AddPoint(1200, 1, 0.5, 0.8);
    opacityFun->AddPoint(4096, 1, 0.5, 0.0);

    // Opacity Gradient
    gradientFun->AddPoint(-3024, 0, 0.5, 0.0);
    gradientFun->AddPoint(100, 0, 0.5, 0.0);
    gradientFun->AddPoint(2700, 1, 0.5, 1);
    gradientFun->AddPoint(4096, 1, 0.5, 1);

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
    prop->SetGradientOpacity(gradientFun);
    volume->SetProperty(prop);

    // Set up the Mapper and connect to the Volume
    mapper->SetInputConnection(connectorPort->GetOutputPort());
    mapper->SetBlendModeToComposite();
    volume->SetMapper(mapper);

    // Add a callback for adding/removing clipping planes with the (A)dd / (D)elete keys
    addRemoveCallback->mapper = mapper;
    addRemoveCallback->prop = volume;
}

BoneAndTissuePipeline::~BoneAndTissuePipeline() {
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
}

void BoneAndTissuePipeline::addToRenderer(vtkRenderer *ren, vtkRenderWindowInteractor *iren) {
    // Set callbacks
    iren->AddObserver(vtkCommand::KeyPressEvent, addRemoveCallback);

    // Add a clipping plane
    plane = ClippingPlaneMaker::AddClippingPlane(iren, volume, mapper);

    // Add the volume to the renderer
    ren->AddVolume(volume);
}

void BoneAndTissuePipeline::removeFromRenderer(vtkRenderer *ren, vtkRenderWindowInteractor *iren) {
    // Remove callbacks
    iren->RemoveObserver(addRemoveCallback);

    // Remove clipping plane
    ClippingPlaneMaker::RemoveClippingPlane(plane, mapper);

    // Remove volume
    ren->RemoveVolume(volume);
}
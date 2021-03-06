#include "BoneOnlyPipeline.h"

BoneOnlyPipeline::BoneOnlyPipeline(itk::ImageSource<ImageType> *input) {
    // Initialize variables (pipeline objects)
    connector = ConnectorType::New();           // To convert from ITK to VTK
    connectorPort = vtkTrivialProducer::New();  //
    volume = vtkVolume::New(); // Volume actor for the image data
    mapper = vtkFixedPointVolumeRayCastMapper::New(); // Mapper for the volume
    thresholdFilter = itk::ThresholdImageFilter<ImageType>::New(); // Threshold segmentation filter
    colorFun = vtkColorTransferFunction::New(); // Transfer functions for color...
    opacityFun = vtkPiecewiseFunction::New();   // opacity...
    prop = vtkVolumeProperty::New(); // Volume property
    addRemoveCallback = ClippingPlaneAddRemoveCallback::New(); // Callback to add or remove clipping planes
    const int boneLevel = 1200; // Threshold intensity for bone

    // Perform threshold segmentation using the bone value
    thresholdFilter->SetInput(input->GetOutput());
    thresholdFilter->ThresholdBelow(boneLevel);
    thresholdFilter->SetOutsideValue(0);
    thresholdFilter->Update();

    // Convert image data from ITK to VTK format
    connector->SetInput(thresholdFilter->GetOutput());

    // Create a dummy producer to provide an output port from the connector, which
    // seems to have no support for an output port
    connectorPort->SetOutput(connector->GetOutput());
    connector->Update();
    connectorPort->Update();

    // Create transfer functions for...
    // Color
    colorFun->AddRGBPoint(-3024, 0, 0, 0, 0.5, 0.0);
    colorFun->AddRGBPoint(860, 1, 0.92, 0.54, 0.5, 0.0);

    // Opacity
    opacityFun->AddPoint(-3024, 0, 0.5, 0.0);
    opacityFun->AddPoint(boneLevel - 1, 0, 0.5, 0.0);
    opacityFun->AddPoint(boneLevel, 1, 0.5, 0.0);

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
    volume->SetProperty(prop);

    // Set up the Mapper and connect to the Volume
    mapper->SetInputConnection(connectorPort->GetOutputPort());
    mapper->SetBlendModeToComposite();
    volume->SetMapper(mapper);

    // Add a callback for adding/removing clipping planes with the (A)dd / (D)elete keys
    addRemoveCallback->mapper = mapper;
    addRemoveCallback->prop = volume;
}

BoneOnlyPipeline::~BoneOnlyPipeline() {
    connectorPort->Delete();
    volume->Delete();
    mapper->Delete();
    prop->Delete();

    // Transfer functions
    colorFun->Delete();
    opacityFun->Delete();

    // Interaction callbacks
    addRemoveCallback->Delete(); // Add or remove clipping planes
}

void BoneOnlyPipeline::addToRenderer(vtkRenderer *ren, vtkRenderWindowInteractor *iren) {
    // Set callbacks
    iren->AddObserver(vtkCommand::KeyPressEvent, addRemoveCallback);

    // Add the volume to the renderer
    ren->AddVolume(volume);
}

void BoneOnlyPipeline::removeFromRenderer(vtkRenderer *ren, vtkRenderWindowInteractor *iren) {
    // Remove callbacks
    iren->RemoveObserver(addRemoveCallback);

    // Remove volume
    ren->RemoveVolume(volume);
}
#pragma once

// Establishes a pipeline to render CT images (from the algorithm given as input) by segmenting
// high-intenity regions and visualizing in 3D.

#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vector>
#include <vtkVolumeProperty.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPlaneWidget.h>
#include <vtkTrivialProducer.h>
#include "SkeletonViz.h"
#include "ClippingPlaneAddRemoveCallback.h"
#include <itkImageSource.h>

class BoneOnlyPipeline {
public:
    BoneOnlyPipeline(itk::ImageSource<ImageType> *input);
    ~BoneOnlyPipeline();
    void addToRenderer(vtkRenderer *ren, vtkRenderWindowInteractor *iren);
    void removeFromRenderer(vtkRenderer *ren, vtkRenderWindowInteractor *iren);

private:
    ConnectorType::Pointer connector;
    vtkTrivialProducer *connectorPort;
    vtkVolume *volume;
    MapperType *mapper;
    ThresholdFilterType::Pointer thresholdFilter;
    vtkVolumeProperty *prop;

    // Transfer functions
    vtkColorTransferFunction *colorFun;
    vtkPiecewiseFunction *opacityFun;

    // Interaction callbacks
    ClippingPlaneAddRemoveCallback *addRemoveCallback; // Add or remove clipping planes

    vtkPlaneWidget *plane; // An initial clipping plane
};
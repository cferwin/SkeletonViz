#pragma once

// Establishes a pipeline to render CT images (from the algorithm given as input) by segmenting
// high-intenity regions and visualizing in 3D.

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
#include <vtkPlanes.h>
#include <vtkClipVolume.h>
#include <vtkUnstructuredGridAlgorithm.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPlaneWidget.h>
#include <vtkPlane.h>
#include <vtkBoxClipDataSet.h>
#include <vtkAlgorithm.h>
#include <vtkTrivialProducer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include "SkeletonViz.h"
#include "ClippingPlaneInteractionCallback.h"
#include "ClippingPlaneMaker.h"
#include "ClippingPlaneAddRemoveCallback.h"
#include "VisualizationModeCallback.h"
#include <itkImageSource.h>

// Define data types
typedef vtkFixedPointVolumeRayCastMapper MapperType;
//typedef itk::Image<unsigned short, 2> ImageType;
typedef itk::Image<double, 3> ImageType;						// vtkFixedPointVolumeRayCastMapper can't support short data
typedef itk::ImageSeriesReader<ImageType> ReaderType;
typedef itk::ImageToVTKImageFilter <ImageType> ConnectorType;	// To convert image data from ITK to VTK format.
typedef itk::NumericSeriesFileNames NameGeneratorType;
typedef itk::ThresholdImageFilter<ImageType> ThresholdFilterType;
class ClippingPlaneAddRemoveCallback;

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
    vtkPiecewiseFunction *gradientFun;

    // Interaction callbacks
    ClippingPlaneAddRemoveCallback *addRemoveCallback; // Add or remove clipping planes
    //VisualizationModeCallback *modeCallback; // Change render mode

    vtkPlaneWidget *plane; // An initial clipping plane
};
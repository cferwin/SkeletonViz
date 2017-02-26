#pragma once
#include "SkeletonViz.h"
#include <vtkCommand.h>
#include "BoneOnlyPipeline.h"

class BoneOnlyPipeline;

// Define a callback for changing the current visualization mode
class VisualizationModeCallback : public vtkCommand {
public:
    static VisualizationModeCallback *New();

    void Execute(vtkObject *caller, unsigned long vtkNotUsed(eventId), void* vtkNotUsed(callData)) VTK_OVERRIDE;

    vtkRenderer *ren;
    vtkRenderWindow *renWin;
    vtkRenderWindowInteractor *iren;
    BoneOnlyPipeline *boneOnly;
};
#pragma once
#include "SkeletonViz.h"
#include <vtkCommand.h>
#include "BoneOnlyPipeline.h"
#include "BoneAndTissuePipeline.h"

// Define a callback for changing the current visualization mode
class VisualizationModeCallback : public vtkCommand {
public:
    static VisualizationModeCallback *New();

    void Execute(vtkObject *caller, unsigned long vtkNotUsed(eventId), void* vtkNotUsed(callData)) VTK_OVERRIDE;

    // These are used both internally and when first starting a pipeline from the main method.
    void ActivateBoneAndTissue();
    void ActivateBoneOnly();
    void DeactivateBoneAndTissue();
    void DeactivateBoneOnly();

    vtkRenderer *ren;
    vtkRenderWindow *renWin;
    vtkRenderWindowInteractor *iren;
    BoneOnlyPipeline *boneOnly;
    BoneAndTissuePipeline *boneAndTissue;

private:
    // Flags to track whether or not the following modes are being rendered.
    // Note these currently will not update if the renderer or interactor
    // are changed. For now, avoid switching rendering systems after
    // initializing an instance of this class.
    bool flagBoneOnly;
    bool flagBoneAndTissue;
};
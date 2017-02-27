#include "VisualizationModeCallback.h"
#include <vtkRenderWindowInteractor.h>

// Define a callback for changing the current visualization mode
VisualizationModeCallback* VisualizationModeCallback::New() {
    VisualizationModeCallback *cb = new VisualizationModeCallback();
    cb->flagBoneOnly = false;
    cb->flagBoneAndTissue = false;

    return cb;
}

void VisualizationModeCallback::Execute(vtkObject *caller, unsigned long vtkNotUsed(eventId), void* vtkNotUsed(callData)) {
    vtkRenderWindowInteractor *iren = static_cast<vtkRenderWindowInteractor*> (caller);

    //Bone only -> Bone and tissue -> both --|
    //    <----------------------------------/
    if (iren->GetKeyCode() == 'c') {
        std::cout << "MODE CHANGE" << std::endl;
        if (currentVisualizationType == BONE_AND_TISSUE) {
            currentVisualizationType = BOTH_MODES;

            ActivateBoneAndTissue();
            ActivateBoneOnly();
        }
        else if (currentVisualizationType == BONE_ONLY) {
            currentVisualizationType = BONE_AND_TISSUE;

            DeactivateBoneOnly();
            ActivateBoneAndTissue();
        }
        else if (currentVisualizationType == BOTH_MODES) {
            currentVisualizationType = BONE_ONLY;

            DeactivateBoneAndTissue();
            ActivateBoneOnly();
        }
    }
}

void VisualizationModeCallback::ActivateBoneAndTissue() {
    if (!flagBoneAndTissue) {
        boneAndTissue->addToRenderer(ren, iren);
        flagBoneAndTissue = true;
    }
}

void VisualizationModeCallback::ActivateBoneOnly() {
    if (!flagBoneOnly) {
        boneOnly->addToRenderer(ren, iren);
        flagBoneOnly = true;
    }
}

void VisualizationModeCallback::DeactivateBoneAndTissue() {
    if (flagBoneAndTissue) {
        boneAndTissue->removeFromRenderer(ren, iren);
        flagBoneAndTissue = false;
    }
}

void VisualizationModeCallback::DeactivateBoneOnly() {
    if (boneOnly) {
        boneOnly->removeFromRenderer(ren, iren);
        flagBoneOnly = false;
    }
}
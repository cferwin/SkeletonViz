#include "VisualizationModeCallback.h"
#include <vtkRenderWindowInteractor.h>

// Define a callback for changing the current visualization mode
VisualizationModeCallback* VisualizationModeCallback::New() {
    return new VisualizationModeCallback();
}

void VisualizationModeCallback::Execute(vtkObject *caller, unsigned long vtkNotUsed(eventId), void* vtkNotUsed(callData)) {
    vtkRenderWindowInteractor *iren = static_cast<vtkRenderWindowInteractor*> (caller);

    if (iren->GetKeyCode() == 'c') {
        std::cout << "MODE CHANGE" << std::endl;
        if (currentVisualizationType == BONE_AND_TISSUE) {
            currentVisualizationType = BONE_ONLY;
            std::cout << "TO BONE ONLY" << std::endl;
            boneOnly->removeFromRenderer(ren, iren);
        }
        else if (currentVisualizationType == BONE_ONLY) {
            currentVisualizationType = BONE_AND_TISSUE;
            std::cout << "TO BONE AND TISSUE" << std::endl;
            boneOnly->addToRenderer(ren, iren);
        }
    }
}
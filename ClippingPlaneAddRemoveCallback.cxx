#pragma once
#include "ClippingPlaneAddRemoveCallback.h"

ClippingPlaneMaker* ClippingPlaneAddRemoveCallback::planeMaker;

// Define a callback for Adding or Removing a clipping plane
ClippingPlaneAddRemoveCallback* ClippingPlaneAddRemoveCallback::New() {
	ClippingPlaneAddRemoveCallback *cb = new ClippingPlaneAddRemoveCallback();
	cb->mapper = NULL;
	cb->prop = NULL;

	return cb;
}

void ClippingPlaneAddRemoveCallback::Execute(vtkObject *caller, unsigned long vtkNotUsed(eventId), void* vtkNotUsed(callData)) {
	vtkRenderWindowInteractor *iren = static_cast<vtkRenderWindowInteractor*> (caller);

	if (iren->GetKeyCode() == 'a') {
		std::cout << "ADD" << std::endl;
		planeMaker->AddClippingPlane(iren, prop, mapper);
	}
	else if (iren->GetKeyCode() == 'd') {
		std::cout << "REM" << std::endl;
		planeMaker->RemoveLastClippingPlane(mapper);
	}
}
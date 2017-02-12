#include "ClippingPlaneInteractionCallback.h"

// Define a callback for modifying a clipping plane
ClippingPlaneInteractionCallback *ClippingPlaneInteractionCallback::New() {
	ClippingPlaneInteractionCallback *cb = new ClippingPlaneInteractionCallback;
	cb->widget = NULL;
	cb->plane = vtkPlane::New();

	return cb;
}

void ClippingPlaneInteractionCallback::Execute(vtkObject *caller, unsigned long vtkNotUsed(eventId), void* vtkNotUsed(callData)) {
	// This method executes when the vtkPlaneWidget is altered, and the
	// plane has probably been altered. Update the mapper to see the change.
	widget = reinterpret_cast<vtkPlaneWidget*>(caller);

	widget->GetPlane(plane);
	mapper->AddClippingPlane(plane);
	mapper->Update();
}

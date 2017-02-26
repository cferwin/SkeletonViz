#pragma once
#include "ClippingPlaneMaker.h"

typedef vtkFixedPointVolumeRayCastMapper MapperType;
class ClippingPlaneMaker;

// Define a callback for Adding or Removing a clipping plane
class ClippingPlaneAddRemoveCallback : public vtkCommand {
public:
	static ClippingPlaneAddRemoveCallback *New();

	void Execute(vtkObject *caller, unsigned long vtkNotUsed(eventId), void* vtkNotUsed(callData)) VTK_OVERRIDE;

	MapperType *mapper = NULL;
	vtkProp3D *prop = NULL;
	static ClippingPlaneMaker *planeMaker;
};
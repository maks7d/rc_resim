// Copyright Epic Games, Inc. All Rights Reserved.

#include "rcsimWheelRear.h"
#include "UObject/ConstructorHelpers.h"

UrcsimWheelRear::UrcsimWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
}
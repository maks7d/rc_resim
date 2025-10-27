// Copyright Epic Games, Inc. All Rights Reserved.

#include "rcsimWheelFront.h"
#include "UObject/ConstructorHelpers.h"

UrcsimWheelFront::UrcsimWheelFront()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	MaxSteerAngle = 40.f;
}
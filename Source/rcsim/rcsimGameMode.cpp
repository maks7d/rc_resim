// Copyright Epic Games, Inc. All Rights Reserved.

#include "rcsimGameMode.h"
#include "rcsimPlayerController.h"

ArcsimGameMode::ArcsimGameMode()
{
	PlayerControllerClass = ArcsimPlayerController::StaticClass();
}

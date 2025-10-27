// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "rcsimPawn.h"
#include "rcsimSportsCar.generated.h"

/**
 *  Sports car wheeled vehicle implementation
 */
UCLASS(abstract)
class ArcsimSportsCar : public ArcsimPawn
{
	GENERATED_BODY()
	
public:

	ArcsimSportsCar();
};

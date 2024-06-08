// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Reversible.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UReversible : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BEATEMUP_API IReversible
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void RecordState() = 0;
	virtual void RewindState() = 0;
};

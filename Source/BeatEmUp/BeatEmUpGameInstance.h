// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BeatEmUpGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BEATEMUP_API UBeatEmUpGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	bool bLoadSave = false;
};

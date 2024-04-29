// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "JumpPoint.generated.h"

/**
 * 
 */
UCLASS()
class BEATEMUP_API AJumpPoint : public ANavLinkProxy
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	UFUNCTION()
		void LinkReached(AActor* Agent, const FVector& Destination);
};

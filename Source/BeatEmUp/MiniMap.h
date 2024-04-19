// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MiniMap.generated.h"

UCLASS()
class BEATEMUP_API AMiniMap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMiniMap();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
		USceneCaptureComponent2D* SceneCaptureComponent;

	UPROPERTY(EditAnywhere, Category="Settings")
		float HeightAbovePlayer = 1000.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

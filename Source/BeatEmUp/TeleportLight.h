// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/Actor.h"
#include "TeleportLight.generated.h"

UCLASS()
class BEATEMUP_API ATeleportLight : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeleportLight();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void UpdateLightProperties(int32 SnapshotIndex) const;

	UPROPERTY(EditAnywhere)
		UPointLightComponent* TeleportLight;

	UPROPERTY(EditAnywhere, Category = "Teleport Light Settings")
		float InitialLightIntensity = 5000.0f;
	UPROPERTY(EditAnywhere, Category = "Teleport Light Settings")
		FLinearColor InitialLightColor = FLinearColor::Red;;
	UPROPERTY(EditAnywhere, Category = "Teleport Light Settings")
		float TargetLightIntensity = 10000.0f;
	UPROPERTY(EditAnywhere, Category = "Teleport Light Settings")
		FLinearColor TargetLightColor = FLinearColor::Blue;
	UPROPERTY(EditAnywhere, Category = "Teleport Light Settings")
		float LightTransitionDuration = 1.0f;;
	UPROPERTY(EditAnywhere)
		float CurrentLightTransitionTime = 0.0f;
	UPROPERTY()
		AActor* TargetActor;
	int32 TotalSnapshots;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/Actor.h"
#include "HealthPack.generated.h"

UCLASS()
class BEATEMUP_API AHealthPack : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHealthPack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
    UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere)
		UPointLightComponent* PointLight;
	UPROPERTY(EditAnywhere)
		UMaterialInterface* HealthMaterialClass;
	UPROPERTY()
		UMaterialInstanceDynamic* HealthMaterialInstance;

	UPROPERTY(EditAnywhere)
		FVector TargetOffset = FVector(0, 0, 50);
	UPROPERTY(EditAnywhere)
		float HealAmount = 50;
	UPROPERTY(EditAnywhere)
		float StartingIntensity = 5000;
	UPROPERTY(EditAnywhere)
		float StartingEmissive = 5;
	UPROPERTY(EditAnywhere)
		float StartingTransparency = 1;
	UPROPERTY(EditAnywhere)
		float MaxLiftTime = 5;

	float CurrentLifeTime;
	FVector InitialPosition;
	FTimerHandle SpawnTimer;

	UFUNCTION()
		void Spawn();
	UFUNCTION()
		void OnMeshOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int BodyIndex, bool bSweep, const FHitResult& Hit);
};

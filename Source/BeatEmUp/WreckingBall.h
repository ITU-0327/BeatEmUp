// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "CableComponent.h"
#include "WreckingBall.generated.h"

UCLASS()
class BEATEMUP_API AWreckingBall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWreckingBall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Wrecking Ball")
		UStaticMeshComponent* TopAnchor;
	UPROPERTY(EditAnywhere, Category = "Wrecking Ball")
		UStaticMeshComponent* BallMesh;
	UPROPERTY(EditAnywhere, Category = "Wrecking Ball")
		UCableComponent* Cable;
	UPROPERTY(EditAnywhere, Category = "Wrecking Ball")
		UPhysicsConstraintComponent* ConstraintComp;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CableComponent.h"
#include "GrapplingHook.generated.h"

UCLASS()
class BEATEMUP_API AGrapplingHook : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrapplingHook();

	void Launch(const FVector& NewTargetLocation, AActor* NewInitiator);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Components")
		UStaticMeshComponent* HookMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Components")
		UCableComponent* CableComponent;

private:
	UPROPERTY(EditAnywhere, Category="Grappling Hook Setting")
		float FlyingSpeed = 3500.0f;
	UPROPERTY(EditAnywhere, Category="Grappling Hook Setting")
		float AttachThreshold = 100.f;
	
	UPROPERTY()
		AActor* Initiator;
	FVector TargetLocation;
	
	bool bRetracting = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

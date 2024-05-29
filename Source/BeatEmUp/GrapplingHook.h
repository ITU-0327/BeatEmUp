// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CableComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GrapplingHook.generated.h"

UCLASS()
class BEATEMUP_API AGrapplingHook : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrapplingHook();

	void Launch(const FVector& NewTargetLocation, AActor* NewInitiator);
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category= "Components")
		UStaticMeshComponent* HookMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Components")
		UProjectileMovementComponent* ProjectileMovement;
	UPROPERTY(EditAnywhere, Category= "Components")
		UCableComponent* CableComponent;

	UPROPERTY(EditAnywhere, Category="Grappling Hook Setting")
		float InitFlingSpeed = 2000.f;
	UPROPERTY(EditAnywhere, Category="Grappling Hook Setting")
		float MaxFlyingSpeed = 3500.0f;
	UPROPERTY(EditAnywhere, Category="Grappling Hook Setting")
		float AttachThreshold = 100.f;
	
	UPROPERTY()
		AActor* Initiator;
	FVector TargetLocation;
	UPROPERTY()
		AActor* TargetActor;
	UPROPERTY()
		UPrimitiveComponent* TargetComponent;

	bool bIsPulling = false;
	bool bRetracting = false;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

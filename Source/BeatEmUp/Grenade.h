// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grenade.generated.h"

UCLASS()
class BEATEMUP_API AGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrenade();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category="Components")
		UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, Category="Granade Effect")
		UParticleSystem* ExplosionEffect;
	UPROPERTY(EditAnywhere, Category="Granade Effect")
		USoundBase* ExplosionSound;
	
	UPROPERTY(EditAnywhere, Category="Granade Settings")
		float ExplosionRadius = 300.f;
	UPROPERTY(EditAnywhere, Category="Granade Settings")
		float ExplosionForce = 2000.f;
	UPROPERTY(EditAnywhere, Category="Granade Settings")
		float TimeToExplode = 3.0f;
	
	UFUNCTION()
		void Initialize(const FVector& ThrowDirection, bool bDebug);
	UFUNCTION()
		void Explode();

private:
	FTimerHandle TimerHandle_Explosion;
	bool bEnableDebug;
};

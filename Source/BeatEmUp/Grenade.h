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
	UPROPERTY(EditAnywhere, Category="Grenade Effect")
		UParticleSystem* ExplosionEffect;
	UPROPERTY(EditAnywhere, Category="Grenade Effect")
		USoundBase* ExplosionSound;
	
	UPROPERTY(EditAnywhere, Category="Grenade Settings")
		float ExplosionRadius = 500.f;
	UPROPERTY(EditAnywhere, Category="Grenade Settings")
		float ExplosionForce = 2500.f;
	UPROPERTY(EditAnywhere, Category="Grenade Settings")
		float ExplosionDamage = 300;
	UPROPERTY(EditAnywhere, Category="Grenade Settings")
		float TimeToExplode = 3.0f;
	
	UFUNCTION()
		void Initialize(const FVector& ThrowDirection, bool bDebug);
	UFUNCTION()
		void Explode();

private:
	FTimerHandle TimerHandle_Explosion;
	bool bEnableDebug;
};

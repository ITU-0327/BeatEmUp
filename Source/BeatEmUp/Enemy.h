// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BeatEmUpCharacter.h"
#include "DamageTextUI.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class BEATEMUP_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY()
		ABeatEmUpCharacter* Player;
	// Health
	UPROPERTY(EditAnywhere, Category="Enemy Settings")
		float MaxHealth = 100;
	UPROPERTY(EditAnywhere)
		float CurrentHealth = MaxHealth;

	// Damage
	UPROPERTY(EditAnywhere, Category="Enemy Settings")
		float DamageToPlayer = 10;
	UPROPERTY(EditAnywhere, Category="Enemy Settings")
		float RagdollTime = 5;
	UPROPERTY(EditAnywhere, Category="Enemy Settings")
		TSubclassOf<UDamageTextUI> DamageTextClass;
	FTimerHandle RagdollTimerHandle;
	
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void DealDamage(float Damage);
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector Normal, const FHitResult& Hit);

	void Ragdoll();
	UFUNCTION()
		void StopRagdoll();
};

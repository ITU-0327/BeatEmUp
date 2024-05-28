// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BeatEmUpCharacter.h"
#include "Bullet.h"
#include "DamageTextUI.h"
#include "DynamicSpotlight.h"
#include "EnemyStatusWidget.h"
#include "HealthPack.h"
#include "Components/WidgetComponent.h"
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

	// Health Bar
	UPROPERTY(EditAnywhere, Category="Enemy Settings")
		TSubclassOf<UEnemyStatusWidget> EnemyStatusWidgetClass;
	UPROPERTY()
		UEnemyStatusWidget* HealthBar;
	UPROPERTY()
		UWidgetComponent* HealthBarWidgetComponent;
	void UpdateWidgetRotation() const;

	// Progression
	UPROPERTY(EditAnywhere, Category="Enemy Settings")
		int ExpAmount = 20;

	// Range Attack
	UPROPERTY(EditAnywhere, Category="Enemy Settings")
		TSubclassOf<ABullet> BulletClass;
	void Shoot(FVector Direction);

	// Death Effect
	UPROPERTY(EditAnywhere, Category="Enemy Settings")
		UNiagaraSystem* DeathParticleClass;
	UPROPERTY(EditAnywhere, Category="Enemy Settings")
		TSubclassOf<AHealthPack> HealthPackClass;

	// Spotlight Effect
	UPROPERTY(EditAnywhere, Category="Enemy Settings")
		TSubclassOf<ADynamicSpotlight> DynamicSpotlightClass;
	UPROPERTY()
		ADynamicSpotlight* Spotlight;
	
	
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

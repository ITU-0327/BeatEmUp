// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

UCLASS()
class BEATEMUP_API APortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal Settings")
		UStaticMeshComponent* Mesh;
	bool bIsPortalActive = false;

	// Portal Sparkle Effect
	UPROPERTY(EditAnywhere, Category = "Portal Settings")
		UNiagaraSystem* PortalSparkleEffect;
	UPROPERTY(EditAnywhere, Category = "Portal Settings")
		FLinearColor ColorMaximum = FLinearColor(0.0f, 0.1f, 1.0f);
	UPROPERTY(EditAnywhere, Category = "Portal Settings")
		FLinearColor ColorMinimum = FLinearColor(0.0f, 1.0f, 1.0f);

	// Portal Material
	UPROPERTY(EditAnywhere, Category = "Portal Settings")
		UMaterialInterface* PortalMaterialClass;
	UPROPERTY()
		UMaterialInstanceDynamic* PortalMaterialInstance;
	UPROPERTY(EditAnywhere, Category = "Portal Settings")
		float InactiveGlowAmount = 3.0f;
	UPROPERTY(EditAnywhere, Category = "Portal Settings")
		float ActiveGlowAmount = 400.0f;
	UPROPERTY(EditAnywhere, Category = "Portal Settings")
		FLinearColor BaseColor = FLinearColor(0.0f, 0.002917f, 0.029167f);

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
							int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void SpawnPortalEffect() const;
	void ActivatePortal();
};

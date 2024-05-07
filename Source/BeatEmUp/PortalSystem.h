// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Portal.h"
#include "FrameData.h"
#include "GameFramework/Actor.h"
#include "PortalSystem.generated.h"

UCLASS()
class BEATEMUP_API APortalSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortalSystem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, Category="Portal Settings")
		TSubclassOf<APortal> PortalClass;
	UPROPERTY(EditAnywhere, Category="Portal Settings")
		float PortalDuration = 30.0f;
	UPROPERTY(EditAnywhere, Category="Portal Settings")
		float PortalRange = 1000.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		AActor* EntryPortal = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		AActor* ExitPortal = nullptr;

	TArray<FFrameData> RecordedFrames;

	void StartPortal(const FVector& EntryLocation, const FRotator& EntryRotation);
	void EndPortal(const FVector& ExitLocation, const FRotator& ExitRotation);
	void RecordFrame();

private:
	void AutoEndPortal();
	FTimerHandle PortalTimerHandle;
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "TeleportLight.h"

// Sets default values
ATeleportLight::ATeleportLight() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TeleportLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("TeleportLight"));
	TeleportLight->Intensity = 0.0f; // Start with the light turned off
	TeleportLight->SetVisibility(true);
	
	RootComponent = TeleportLight;
}

// Called when the game starts or when spawned
void ATeleportLight::BeginPlay() {
	Super::BeginPlay();

	TargetActor = GetWorld()->GetFirstPlayerController()->GetPawn();

	TeleportLight->SetIntensity(InitialLightIntensity);
	TeleportLight->SetLightColor(InitialLightColor);
}

// Called every frame
void ATeleportLight::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if(!TargetActor) return;
	
	SetActorLocation(TargetActor->GetActorLocation());
}

void ATeleportLight::UpdateLightProperties(int32 SnapshotIndex) const {
	if(TotalSnapshots > 0) {
		const float Alpha = static_cast<float>(SnapshotIndex) / TotalSnapshots;
		const float NewIntensity = FMath::Lerp(InitialLightIntensity, TargetLightIntensity, Alpha);
		FLinearColor NewColor = FMath::Lerp(InitialLightColor, TargetLightColor, Alpha);
		TeleportLight->SetIntensity(NewIntensity);
		TeleportLight->SetLightColor(NewColor);
	}
}

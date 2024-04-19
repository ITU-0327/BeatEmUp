// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniMap.h"

#include "BeatEmUpCharacter.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMiniMap::AMiniMap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Scene Capture Component 2D"));
	SceneCaptureComponent->ProjectionType = ECameraProjectionMode::Orthographic;
	SceneCaptureComponent->OrthoWidth = 1024.0f;
	SceneCaptureComponent->SetWorldRotation(FRotator(-90.f, 0.f, 0.f));

	RootComponent = SceneCaptureComponent;
}

// Called when the game starts or when spawned
void AMiniMap::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMiniMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(ABeatEmUpCharacter* Character = Cast<ABeatEmUpCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0))) {
		const FVector NewLocation = Character->GetActorLocation() + FVector(0, 0, HeightAbovePlayer);
		SetActorLocation(NewLocation);

		UserWidget = Character->InGameUI;
		if(UserWidget) {
			float Rotation = Character->GetActorRotation().Yaw;
			UserWidget->UpdatePlayerIndicator(Rotation);
		}
	}
}

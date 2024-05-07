#pragma once

#include "CoreMinimal.h"
#include "FrameData.generated.h"

USTRUCT(BlueprintType)
struct FFrameData {
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Frame Data")
	FVector Location;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Frame Data")
	FRotator Rotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Frame Data")
	float Timestamp;

	FFrameData() : Location(FVector::ZeroVector), Rotation(FRotator::ZeroRotator), Timestamp(0.0f) {}

	FFrameData(FVector InPosition, FRotator InRotation, float InTimestamp)
		: Location(InPosition), Rotation(InRotation), Timestamp(InTimestamp) {}
};

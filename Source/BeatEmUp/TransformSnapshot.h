#pragma once

#include "CoreMinimal.h"
#include "TransformSnapshot.generated.h"

USTRUCT(BlueprintType)
struct FTransformSnapshot {
	GENERATED_BODY()

	UPROPERTY(Transient)
	FTransform Transform;

	FTransformSnapshot() = default;
	FTransformSnapshot(const FTransform& InTransform)
		: Transform(InTransform) {}
};

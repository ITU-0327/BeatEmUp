// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BeatEmUpSaveGame.h"
#include "Enemy.h"
#include "GameFramework/GameModeBase.h"
#include "BeatEmUpGameMode.generated.h"

UCLASS(minimalapi)
class ABeatEmUpGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABeatEmUpGameMode();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AEnemy> EnemyClass;

	void RecordAllReversibleActors() const;
	void RewindAllReversibleActors() const;
	void SetRewindStatusForEnemies(bool bIsRewinding);
	void Load(UBeatEmUpSaveGame* LoadedGame) const;
	void Save(UBeatEmUpSaveGame* SaveGame) const;
	UFUNCTION()
		void PostBeginPlay();
};




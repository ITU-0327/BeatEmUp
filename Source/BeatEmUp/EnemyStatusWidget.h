// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "EnemyStatusWidget.generated.h"

class AEnemy;

/**
 * 
 */
UCLASS()
class BEATEMUP_API UEnemyStatusWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta=(BindWidget))
		UImage* WarningIcon;
	UPROPERTY(meta=(BindWidget))
		UProgressBar* HealthBar;

	UPROPERTY()
		AEnemy* Enemy;

	void UpdateValues() const;
	void ShowWarningIcon() const;
	void HideWarningIcon() const;
};

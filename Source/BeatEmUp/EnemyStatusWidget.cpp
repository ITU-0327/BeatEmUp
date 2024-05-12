// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyStatusWidget.h"
#include "Enemy.h"

void UEnemyStatusWidget::UpdateValues() const {
	if(!Enemy) return;
	if(!HealthBar) return;
	
	HealthBar->SetPercent(Enemy->CurrentHealth / Enemy->MaxHealth);
}

void UEnemyStatusWidget::ShowWarningIcon() const {
	if(WarningIcon) WarningIcon->SetVisibility(ESlateVisibility::Visible);
	HideInvestigateIcon();
}
void UEnemyStatusWidget::HideWarningIcon() const {
	if(WarningIcon) WarningIcon->SetVisibility(ESlateVisibility::Hidden);
}

void UEnemyStatusWidget::ShowInvestigateIcon() const {
	if(InvestigateIcon) InvestigateIcon->SetVisibility(ESlateVisibility::Visible);
	HideWarningIcon();
}

void UEnemyStatusWidget::HideInvestigateIcon() const {
	if(InvestigateIcon) InvestigateIcon->SetVisibility(ESlateVisibility::Hidden);
}



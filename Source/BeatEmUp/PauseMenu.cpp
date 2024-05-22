// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenu.h"

#include "BeatEmUpGameMode.h"
#include "BeatEmUpSaveGame.h"
#include "Kismet/GameplayStatics.h"

void UPauseMenu::NativeOnInitialized() {
	Super::NativeOnInitialized();

	ContinueButton->OnClicked.AddDynamic(this, &UPauseMenu::OnContinueClicked);
	SaveButton->OnClicked.AddDynamic(this, &UPauseMenu::OnSaveClicked);
	QuitButton->OnClicked.AddDynamic(this, &UPauseMenu::OnQuitClicked);
}

void UPauseMenu::OnContinueClicked() {
	UGameplayStatics::SetGamePaused(GetWorld(), false);
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(false);
	GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeGameOnly());
	RemoveFromParent();
}

void UPauseMenu::OnSaveClicked() {
	UBeatEmUpSaveGame* SaveGameInstance = Cast<UBeatEmUpSaveGame>(UGameplayStatics::CreateSaveGameObject(UBeatEmUpSaveGame::StaticClass()));
	if(!SaveGameInstance) return;
	ABeatEmUpGameMode* BeatEmUpGameMode = Cast<ABeatEmUpGameMode>(GetWorld()->GetAuthGameMode());
	if(!BeatEmUpGameMode) return;

	BeatEmUpGameMode->Save(SaveGameInstance);
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, "SaveSlot", 0);
}

void UPauseMenu::OnQuitClicked() {
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, true);
}

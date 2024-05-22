// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadButton.h"

#include "BeatEmUpGameInstance.h"
#include "Kismet/GameplayStatics.h"

ULoadButton::ULoadButton() {
	OnClicked.AddDynamic(this, &ULoadButton::OnClick);
}

void ULoadButton::OnClick() {
	if(UGameplayStatics::DoesSaveGameExist("TestSave", 0)) {
		UGameplayStatics::OpenLevel(GetWorld(), MapToOpen);
		if(UBeatEmUpGameInstance* GameInstance = Cast<UBeatEmUpGameInstance>(GetWorld()->GetGameInstance()))
			GameInstance->bLoadSave = true;
	}
}

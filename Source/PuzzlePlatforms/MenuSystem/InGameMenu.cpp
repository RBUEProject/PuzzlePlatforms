// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMenu.h"
#include "Components/Button.h"


bool UInGameMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success)return false;
	if (!ensure(CancelButton != nullptr)) return false;
	CancelButton->OnClicked.AddDynamic(this, &UInGameMenu::CancelPressed);
	if (!ensure(QuitButton != nullptr)) return false;
	QuitButton->OnClicked.AddDynamic(this, &UInGameMenu::QuitPressed);
	return true;
}

void UInGameMenu::CancelPressed()
{
	TearDown();
	//UE_LOG(LogTemp, Warning, TEXT("CancelPressed"));
}

void UInGameMenu::QuitPressed()
{
	if(!ensure(MenuInterface!=nullptr))return;
	TearDown();
	MenuInterface->LoadMainMenu();
	//UE_LOG(LogTemp, Warning, TEXT("QuitPressed"));
}

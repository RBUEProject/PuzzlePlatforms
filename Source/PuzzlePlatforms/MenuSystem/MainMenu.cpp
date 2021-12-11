// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"

void UMainMenu::SetMenuInterface(IMenuInterface* menuInterface)
{
	MenuInterface = menuInterface;
}

void UMainMenu::Setup()
{
	this->AddToViewport();
	UWorld*World = GetWorld();
	if(!ensure(World!=nullptr))return;
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr))return;
	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(this->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = true;
}

void UMainMenu::TearDown()//保证之前没有建立过，建立过就清除，然后重新建立
{
	this->RemoveFromViewport();
	UWorld*World = GetWorld();
	if (!ensure(World != nullptr))return;
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr))return;
	FInputModeGameOnly InputModeData;

	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = false;
}

bool UMainMenu::Initialize()
{
	bool Success = Super::Initialize();
	if(!Success)return false;
	if(!ensure(Host!=nullptr)) return false;
	Host->OnClicked.AddDynamic(this,&UMainMenu::HostServer);
	if (!ensure(Join != nullptr)) return false;
	Join->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);
	if (!ensure(Back != nullptr)) return false;
	Back->OnClicked.AddDynamic(this, &UMainMenu::BackToMainMenu);
	return true;
}

void UMainMenu::HostServer()
{
	if (MenuInterface != nullptr)
	{
		MenuInterface->Host();
	}
}

void UMainMenu::OpenJoinMenu()
{
	if(!ensure(MenuSwitcher !=nullptr))return;
	if (!ensure(JoinMenu != nullptr))return;
	MenuSwitcher->SetActiveWidget(JoinMenu);
	UE_LOG(LogTemp,Warning,TEXT("OpenJoinMenu"));
}

void UMainMenu::BackToMainMenu()
{
	if (!ensure(MenuSwitcher != nullptr))return;
	if (!ensure(MainMenu != nullptr))return;
	MenuSwitcher->SetActiveWidget(MainMenu);
}

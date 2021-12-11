// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlatformInstance.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "MenuSystem/MainMenu.h"
UPuzzlePlatformInstance::UPuzzlePlatformInstance(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget>MenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));
	if(!ensure(MenuBPClass.Class!=nullptr))return;
	MenuClass = MenuBPClass.Class;
}

void UPuzzlePlatformInstance::Init()
{
	
	UE_LOG(LogTemp, Warning, TEXT("Found Class %s"), *MenuClass->GetName());
}

void UPuzzlePlatformInstance::LoadMenu()
{
	if(!ensure(MenuClass!=nullptr))return;
	Menu = CreateWidget<UMainMenu>(this,MenuClass);
	if (!ensure(Menu != nullptr))return;
	Menu->Setup();
	Menu->SetMenuInterface(this);
}

void UPuzzlePlatformInstance::Host()
{
	if (Menu != nullptr) {
		Menu->TearDown();
	}
	UEngine*Engine = GetEngine();
	if(!ensure(Engine!=nullptr))return;
	Engine->AddOnScreenDebugMessage(0,2,FColor::Green,TEXT("Hosting"));
	UWorld*World = GetWorld();
	if (!ensure(World != nullptr))return;

	World->ServerTravel("/Game/ThirdPersonCPP/Maps/ThirdPersonExampleMap?listen");//将玩家控制器传送到某个地图,并监听

}

void UPuzzlePlatformInstance::Join(const FString& Address)
{
	UEngine*Engine = GetEngine();
	if (!ensure(Engine != nullptr))return;
	Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, FString::Printf(TEXT("Joining %s"),*Address));
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr))return;
	PlayerController->ClientTravel(Address,ETravelType::TRAVEL_Absolute);
}

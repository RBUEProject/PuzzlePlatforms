// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlatformInstance.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "MenuSystem/MenuWidget.h"
#include "MenuSystem/MainMenu.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

const static FName SESSION_NAME = TEXT("My Session Game");
UPuzzlePlatformInstance::UPuzzlePlatformInstance(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget>MenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));
	if(!ensure(MenuBPClass.Class!=nullptr))return;
	MenuClass = MenuBPClass.Class;

	ConstructorHelpers::FClassFinder<UUserWidget>InGameMenuBPClass(TEXT("/Game/MenuSystem/WBP_InGameMenu"));
	if (!ensure(InGameMenuBPClass.Class != nullptr))return;
	InGameMenuClass = InGameMenuBPClass.Class;
}

void UPuzzlePlatformInstance::Init()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found %s Subsystem "), *Subsystem->GetSubsystemName().ToString());
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid()) {
			//UE_LOG(LogTemp, Warning, TEXT("Found  Session "));
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this,&UPuzzlePlatformInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this,&UPuzzlePlatformInstance::OnDestroySessionComplete);
		}
	}
	//else {
	//	UE_LOG(LogTemp, Warning, TEXT("Found no subsystem"));
	//}
}

void UPuzzlePlatformInstance::LoadMenuWidget()
{
	if(!ensure(MenuClass!=nullptr))return;
	Menu = CreateWidget<UMainMenu>(this,MenuClass);
	if (!ensure(Menu != nullptr))return;
	Menu->Setup();
	Menu->SetMenuInterface(this);
}

void UPuzzlePlatformInstance::InGameLoadMenu()
{
	if (!ensure(InGameMenuClass != nullptr))return;
	UMenuWidget*MenuWidget = CreateWidget<UMenuWidget>(this, InGameMenuClass);
	if (!ensure(MenuWidget != nullptr))return;
	MenuWidget->Setup();
	MenuWidget->SetMenuInterface(this);
}

void UPuzzlePlatformInstance::Host()
{
	auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
	if (ExistingSession != nullptr) {
		SessionInterface->DestroySession(SESSION_NAME);
	}
	else {
		CreateSession();
	}
}

void UPuzzlePlatformInstance::Join(const FString& Address)
{
	if (Menu != nullptr) {
		Menu->TearDown();
	}
	UEngine*Engine = GetEngine();
	if (!ensure(Engine != nullptr))return;
	Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, FString::Printf(TEXT("Joining %s"),*Address));
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr))return;
	PlayerController->ClientTravel(Address,ETravelType::TRAVEL_Absolute);
}


void UPuzzlePlatformInstance::LoadMainMenu()
{
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr))return;
	PlayerController->ClientTravel("/Game/MenuSystem/MainMenu", ETravelType::TRAVEL_Absolute);
}

void UPuzzlePlatformInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if(!Success)return;
	if (Menu != nullptr) {
		Menu->TearDown();
	}
	UEngine*Engine = GetEngine();
	if (!ensure(Engine != nullptr))return;
	Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));
	UWorld*World = GetWorld();
	if (!ensure(World != nullptr))return;

	World->ServerTravel("/Game/ThirdPersonCPP/Maps/ThirdPersonExampleMap?listen");//将玩家控制器传送到某个地图,并监听
}

void UPuzzlePlatformInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success) {
		CreateSession();
	}
}

void UPuzzlePlatformInstance::CreateSession()
{
	if (SessionInterface.IsValid()) {
		FOnlineSessionSettings SessionSettings;
		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);//第0个玩家,创建会话
	}
}

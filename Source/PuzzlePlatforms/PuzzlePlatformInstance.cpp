// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlatformInstance.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "MenuSystem/MenuWidget.h"
#include "MenuSystem/MainMenu.h"
#include "OnlineSessionSettings.h"

const static FName SESSION_NAME = TEXT("GameSession");
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");


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
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this,&UPuzzlePlatformInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this,&UPuzzlePlatformInstance::OnJoinSessionComplete);
		}
	}

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

void UPuzzlePlatformInstance::Host(FString ServerName)
{
	DesiredServerName = ServerName;
	if (SessionInterface.IsValid()) {
		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if (ExistingSession != nullptr) {
			SessionInterface->DestroySession(SESSION_NAME);
		}
		else {
			CreateSession();
		}
	}
}

void UPuzzlePlatformInstance::Join(uint32 Index)
{
	if(!SessionInterface.IsValid())return;
	if(!SessionSearch.IsValid())return;
	if (Menu != nullptr) {
		Menu->TearDown();
	}
	SessionInterface->JoinSession(0,SESSION_NAME,SessionSearch->SearchResults[Index]);
}


void UPuzzlePlatformInstance::LoadMainMenu()
{
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr))return;
	PlayerController->ClientTravel("/Game/MenuSystem/MainMenu", ETravelType::TRAVEL_Absolute);
}

void UPuzzlePlatformInstance::RefreshSessionList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid()) {
		//SessionSearch->bIsLanQuery = true;
		SessionSearch->MaxSearchResults = 100;//最大搜索数
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE,true,EOnlineComparisonOp::Equals);
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
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

	World->ServerTravel("/Game/ThirdPersonCPP/Maps/Lobby?listen");//将玩家控制器传送到某个地图,并监听
}

void UPuzzlePlatformInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success) {
		CreateSession();
	}
}

void UPuzzlePlatformInstance::OnFindSessionsComplete(bool Success)
{
	if (Success&&SessionSearch.IsValid()&& Menu!=nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Found Session"));
		TArray<FServerData> ServerNames;

		for(const FOnlineSessionSearchResult& SearchResult:SessionSearch->SearchResults){
			UE_LOG(LogTemp, Warning, TEXT("Found Session named : %s"),*SearchResult.GetSessionIdStr());
			FServerData Data;
			Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
			Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
			Data.HostUserName = SearchResult.Session.OwningUserName;
			FString ServerName;
			if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))
			{
				Data.Name = ServerName;

			}
			else
			{
				Data.Name = "Could not find name.";
			}

			ServerNames.Add(Data);
		}
		Menu->SetServerList(ServerNames);
	}
}

void UPuzzlePlatformInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if(!SessionInterface.IsValid())return;
	FString Address;
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address)) {
		UE_LOG(LogTemp,Warning,TEXT("Can not Get Connect String"));
		return;
	}
	UEngine*Engine = GetEngine();
	if (!ensure(Engine != nullptr))return;
	Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr))return;
	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}

void UPuzzlePlatformInstance::CreateSession()
{
	if (SessionInterface.IsValid()) {
		FOnlineSessionSettings SessionSettings;
		if(IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
		{
			SessionSettings.bIsLANMatch = true;//本地连接
		}
		else 
		{
			SessionSettings.bIsLANMatch = false;
		}
		SessionSettings.NumPublicConnections = 2;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.Set(SERVER_NAME_SETTINGS_KEY,DesiredServerName,EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		//SessionSettings.bUseLobbiesIfAvailable = true;//用于连接Steam
		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);//第0个玩家,创建会话
	}
}

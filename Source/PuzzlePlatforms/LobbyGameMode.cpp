// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "TimerManager.h"
#include "PuzzlePlatformInstance.h"
void ALobbyGameMode::PostLogin(APlayerController*NewPlayer)
{
	Super::PostLogin(NewPlayer);
	++NUmberOfPlayers;
	if (NUmberOfPlayers >= 2)
	{
		GetWorldTimerManager().SetTimer(GameStartTimer,this, &ALobbyGameMode::StartGame,5);
		
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	--NUmberOfPlayers;
}

void ALobbyGameMode::StartGame()
{
	auto GameInstance = Cast<UPuzzlePlatformInstance>(GetGameInstance());
	if(GameInstance == nullptr)return;
	GameInstance->StartSession();
	UWorld*World = GetWorld();
	if (!ensure(World != nullptr))return;
	bUseSeamlessTravel = true;
	World->ServerTravel("/Game/ThirdPersonCPP/Maps/Game?listen");
}

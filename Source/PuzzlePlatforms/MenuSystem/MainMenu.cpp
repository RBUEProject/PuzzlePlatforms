// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "UObject/ConstructorHelpers.h"
#include "ServerRow.h"
#include "Components/TextBlock.h"
UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer) {
	ConstructorHelpers::FClassFinder<UUserWidget>ServerRowBPClass(TEXT("/Game/MenuSystem/WBP_ServerRow"));
	if (!ensure(ServerRowBPClass.Class != nullptr))return;
	ServerRowClass = ServerRowBPClass.Class;
}



bool UMainMenu::Initialize()
{
	bool Success = Super::Initialize();
	if(!Success)return false;

	if(!ensure(Host!=nullptr)) return false;
	Host->OnClicked.AddDynamic(this,&UMainMenu::OpenHostMenu);

	if (!ensure(CancelHostMenuButton != nullptr)) return false;
	CancelHostMenuButton->OnClicked.AddDynamic(this, &UMainMenu::BackToMainMenu);

	if (!ensure(Back != nullptr)) return false;
	Join->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	if (!ensure(Back != nullptr)) return false;
	Back->OnClicked.AddDynamic(this, &UMainMenu::BackToMainMenu);

	if (!ensure(ConfirmJoinButton != nullptr)) return false;
	ConfirmJoinButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);

	if (!ensure(ConfirmHostButton != nullptr)) return false;
	ConfirmHostButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

	if (!ensure(ExitButton != nullptr)) return false;
	ExitButton->OnClicked.AddDynamic(this, &UMainMenu::ExitGame);

	return true;
}

void UMainMenu::HostServer()
{
	if (MenuInterface != nullptr)
	{
		FString ServerName = ServerHostName->Text.ToString();
		MenuInterface->Host(ServerName);
	}
}

void UMainMenu::OpenJoinMenu()
{
	if(!ensure(MenuSwitcher !=nullptr))return;
	if (!ensure(JoinMenu != nullptr))return;
	MenuSwitcher->SetActiveWidget(JoinMenu);
	UE_LOG(LogTemp,Warning,TEXT("OpenJoinMenu"));
	if (MenuInterface != nullptr)
	{
		MenuInterface->RefreshSessionList();
	}
}

void UMainMenu::OpenHostMenu()
{
	MenuSwitcher->SetActiveWidget(HostMenu);
}

void UMainMenu::BackToMainMenu()
{
	if (!ensure(MenuSwitcher != nullptr))return;
	if (!ensure(MainMenu != nullptr))return;
	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UMainMenu::SetServerList(TArray<FServerData>ServerNames)
{
	UWorld* World = this->GetWorld();
	if (!ensure(World != nullptr)) return;
	//ServerList->ClearChildren();
	uint32 i = 0;
	for (const FServerData& ServerData : ServerNames) {
		UServerRow* Row = CreateWidget<UServerRow>(World, ServerRowClass);
		if (!ensure(Row != nullptr))return;
		Row->Setup(this, i);
		Row->ServerName->SetText(FText::FromString(ServerData.Name));
		Row->HostUser->SetText(FText::FromString(ServerData.HostUserName));
		FString FractionText = FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers);
		Row->ConnectionFraction->SetText(FText::FromString(FractionText));
		ServerList->AddChild(Row);
		++i;
	}
}

void UMainMenu::SelectIndex(uint32 Index)
{
	SelectedIndex = Index;
	UpdateChildren();
}

void UMainMenu::JoinServer()
{
	if (SelectedIndex.IsSet()&& MenuInterface != nullptr) {
		UE_LOG(LogTemp,Warning,TEXT("Select index %d"),SelectedIndex.GetValue());
		MenuInterface->Join(SelectedIndex.GetValue());
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Select index not set."));
	}
}

void UMainMenu::ExitGame()
{
	UWorld*World = GetWorld();
	if (!ensure(World != nullptr))return;
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr))return;
	PlayerController->ConsoleCommand("quit");
}

void UMainMenu::UpdateChildren()
{
	for (int32 i = 0; i < ServerList->GetChildrenCount(); ++i)
	{
		auto Row = Cast<UServerRow>(ServerList->GetChildAt(i));
		if (Row != nullptr)
		{
			Row->Selected = (SelectedIndex.IsSet()&&SelectedIndex.GetValue() == i);
		}
	}
}

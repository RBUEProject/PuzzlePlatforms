// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"

#include "MainMenu.generated.h"


UCLASS()
class PUZZLEPLATFORMS_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()

public:
	UMainMenu(const FObjectInitializer& ObjectInitializer);
	void SetServerList(TArray<FString>ServerNames);
	void SelectIndex(uint32 Index);
protected:
	virtual bool Initialize();

private:

	TSubclassOf<class UUserWidget>ServerRowClass;

	UPROPERTY(meta = (BindWidget))
	class UButton* Host; 
	UPROPERTY(meta = (BindWidget))
	class UButton* Join;
	UPROPERTY(meta = (BindWidget))
	class UButton* Back;
	UPROPERTY(meta = (BindWidget))
	class UButton* ExitButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* ConfirmJoinButton;
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	class UWidget* JoinMenu;
	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu;
	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ServerList;


	UFUNCTION()
	void HostServer();
	UFUNCTION()
	void OpenJoinMenu();
	UFUNCTION()
	void BackToMainMenu();
	UFUNCTION()
	void JoinServer();
	UFUNCTION()
	void ExitGame();

	TOptional<uint32>SelectedIndex;
};
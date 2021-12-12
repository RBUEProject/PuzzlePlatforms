// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuInterface.h"
#include "MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetMenuInterface(IMenuInterface* menuInterface);
	void Setup();
	void TearDown();
protected:
	virtual bool Initialize();


private:
	UPROPERTY(meta = (BindWidget))
	class UButton* Host; 
	UPROPERTY(meta = (BindWidget))
	class UButton* Join;
	UPROPERTY(meta = (BindWidget))
	class UButton* Back;
	UPROPERTY(meta = (BindWidget))
	class UButton* ConfirmJoinButton;
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	class UWidget* JoinMenu;
	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu;
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* IPAddressField;


	UFUNCTION()
	void HostServer();
	UFUNCTION()
	void OpenJoinMenu();
	UFUNCTION()
	void BackToMainMenu();
	UFUNCTION()
	void JoinServer();

	IMenuInterface* MenuInterface;
};
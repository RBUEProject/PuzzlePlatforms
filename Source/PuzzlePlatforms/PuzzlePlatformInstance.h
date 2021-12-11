// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuSystem/MenuInterface.h"
#include "PuzzlePlatformInstance.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API UPuzzlePlatformInstance : public UGameInstance,public IMenuInterface
{
	GENERATED_BODY()

public:
	UPuzzlePlatformInstance(const FObjectInitializer& ObjectInitializer); 
	
	virtual void Init();

	UFUNCTION(Exec,BlueprintCallable)//控制台命令
	void LoadMenu();

	UFUNCTION(Exec)//控制台命令
	void Host();

	UFUNCTION(Exec)//控制台命令
	void Join(const FString& Address);

private:
	TSubclassOf<class UUserWidget>MenuClass;

	class UMainMenu*Menu;
};

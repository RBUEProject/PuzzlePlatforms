// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PuzzlePlatformInstance.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API UPuzzlePlatformInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPuzzlePlatformInstance(const FObjectInitializer& ObjectInitializer); 
	
	virtual void Init();

	UFUNCTION(Exec)//控制台命令
	void Host();

	UFUNCTION(Exec)//控制台命令
	void Join(const FString& Address);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameModeNew.generated.h"

/**
 * 
 */
UCLASS()
class DEATHROCKET_TPS_API AGameModeNew : public AGameModeBase
{
	GENERATED_BODY()
	
public :
	UFUNCTION(BlueprintImplementableEvent)
	void FirstTeamWin();

	UFUNCTION(BlueprintImplementableEvent)
	void SecondTeamWin();

	UFUNCTION(BlueprintImplementableEvent)
	void Draw();

	UFUNCTION(BlueprintImplementableEvent)
	void SelectPlayerTraining();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "../MyPlayer.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TeamsManager.generated.h"
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class DEATHROCKET_TPS_API UTeamsManager : public UObject
{
	GENERATED_BODY()
public:
	int scoreTeam1 = 0;
	int scoreTeam2 = 0;

	UFUNCTION(BlueprintCallable, Category = "Functions")
	void Init1vs1();

	UFUNCTION(BlueprintCallable, Category = "Functions")
	void Init2vs2();

	UFUNCTION(BlueprintCallable, Category = "Functions")
	void EndGame();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Personall, meta = (AllowPrivateAccess = "true"))
	TArray<AMyPlayer*> players;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Personall, meta = (AllowPrivateAccess = "true"))
	FVector firstPlayerPos;

	bool firstGame = false;

	void CreatePlayers(int nums);

	UFUNCTION(BlueprintImplementableEvent)
	void SetPseudos(int nums);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Personall, meta = (AllowPrivateAccess = "true"))
	int readyPlayers = 0;

	UFUNCTION(BlueprintCallable, Category = "Functions")
	void DestroyPlayers();
};

template <typename T>
void GetAllActorsOfType(UWorld* World, TArray<T*>& OutActors);
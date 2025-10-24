// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Public/IPlayerUlti.h"
#include "PlayerUltis.generated.h"

UCLASS()
// ######################### Shield Ulti #########################
class DEATHROCKET_TPS_API UPlayerUltiIncreaseShield : public UIPlayerUlti
{
	GENERATED_BODY()

public:
	UPlayerUltiIncreaseShield() = default;
	~UPlayerUltiIncreaseShield() = default;

	virtual void StartUlti(AMyPlayer* _player) override;
	virtual void UpdateUlti(AMyPlayer* _player, float _deltaTime) override;

private:
	float shieldAddValue = 25.f;
	virtual void EndUlti(AMyPlayer* _player) override;
};

// ######################### Health Ulti #########################
UCLASS()
class DEATHROCKET_TPS_API UPlayerUltiIncreaseHealth : public UIPlayerUlti
{
	GENERATED_BODY()

public:
	UPlayerUltiIncreaseHealth() = default;
	~UPlayerUltiIncreaseHealth() = default;

	virtual void StartUlti(AMyPlayer* _player) override;
	virtual void UpdateUlti(AMyPlayer* _player, float _deltaTime) override;

private:
	float HealthAddValue = 25.f;

	virtual void EndUlti(AMyPlayer* _player) override;
};

// ######################### Infinite Rocket Ulti #########################
UCLASS()
class DEATHROCKET_TPS_API UPlayerUltiInfiniteRocket : public UIPlayerUlti
{
	GENERATED_BODY()

public:
	UPlayerUltiInfiniteRocket();
	~UPlayerUltiInfiniteRocket() = default;
	
	virtual void StartUlti(AMyPlayer* _player) override;
	virtual void UpdateUlti(AMyPlayer* _player, float _deltaTime) override;

private:
	float duration = 5.f;
	float timerDuration = 0.f;

	virtual void EndUlti(AMyPlayer* _player) override;
};
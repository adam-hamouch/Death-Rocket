// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerUltis.h"
#include "../MyPlayer.h"


// ######################### Shield Ulti #########################
void UPlayerUltiIncreaseShield::StartUlti(AMyPlayer* _player)
{
	_player->RegenerateShield(shieldAddValue);
	EndUlti(_player);
}

void UPlayerUltiIncreaseShield::UpdateUlti(AMyPlayer* _player, float _deltaTime)
{
}

void UPlayerUltiIncreaseShield::EndUlti(AMyPlayer* _player)
{
	_player->DisableUlti();
	bIsToDelete = true;
}

// ######################### Health Ulti #########################
void UPlayerUltiIncreaseHealth::StartUlti(AMyPlayer* _player)
{
	_player->RegenerateHealth(HealthAddValue);
	EndUlti(_player);
}

void UPlayerUltiIncreaseHealth::UpdateUlti(AMyPlayer* _player, float _deltaTime)
{
}

void UPlayerUltiIncreaseHealth::EndUlti(AMyPlayer* _player)
{
	_player->DisableUlti();
	bIsToDelete = true;
}


// ######################### Infinite Rocket Ulti #########################
UPlayerUltiInfiniteRocket::UPlayerUltiInfiniteRocket()
{
	timerDuration = duration;
}

void UPlayerUltiInfiniteRocket::StartUlti(AMyPlayer* _player)
{
	_player->SetNbRocketLoader(_player->GetNbMaxRocketLoader());
	_player->SetIsReloading(false);
}

void UPlayerUltiInfiniteRocket::UpdateUlti(AMyPlayer* _player, float _deltaTime)
{
	timerDuration -= _deltaTime;
	if (timerDuration >= 0.f)
	{
		_player->ResetNbRocket();
		_player->OnRocketChangedDelegate.Broadcast(-1);
	}
	else
	{
		EndUlti(_player);
	}
}

void UPlayerUltiInfiniteRocket::EndUlti(AMyPlayer* _player)
{
	_player->DisableUlti();
	_player->OnRocketChangedDelegate.Broadcast(_player->GetNbRocketLoader());
	bIsToDelete = true;
}
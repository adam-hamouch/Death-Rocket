// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IPlayerUlti.generated.h"

class AMyPlayer;

UCLASS(Abstract)
class DEATHROCKET_TPS_API UIPlayerUlti : public UObject
{
	GENERATED_BODY()
public:
	UIPlayerUlti() = default;
	~UIPlayerUlti() = default;

	bool bIsToDelete = false;

	virtual void StartUlti(AMyPlayer* _player) { check(0 && "You must override this"); };

	virtual void UpdateUlti(AMyPlayer* _player, float _deltaTime) { check(0 && "You must override this"); };

protected:
	virtual void EndUlti(AMyPlayer* _player) { check(0 && "You must override this"); };
	
};

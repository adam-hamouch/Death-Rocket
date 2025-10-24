// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Buff.generated.h"

class AMyPlayer;

UCLASS(Abstract)
class DEATHROCKET_TPS_API UBuff : public UObject
{
	GENERATED_BODY()

public:
	UBuff() = default;
	~UBuff() = default;

	bool bIsToDelete = false;

	virtual void StartBuff(AMyPlayer* _player) { check(0 && "You must override this"); };
	virtual void UpdateBuff(AMyPlayer* _player, float _deltaTime) { check(0 && "You must override this"); };

protected:
	virtual void EndBuff(AMyPlayer* _player) { check(0 && "You must override this"); };
};

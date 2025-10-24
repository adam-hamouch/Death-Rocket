#pragma once

#include "MyPlayer.h"
#include "Components/SphereComponent.h"
#include "Gameframework/CharacterMovementComponent.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Explosion.generated.h"

UCLASS()
class DEATHROCKET_TPS_API AExplosion : public AActor
{
	GENERATED_BODY()

public:
	AExplosion();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* ExplosionSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Particles")
	FVector ExplosionScale;

#pragma region Particles
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Particles")
	UParticleSystem* ExplosionParticles = nullptr;
#pragma endregion

protected:
	virtual void BeginPlay() override;

private:
	TSet<AActor*> ProcessedActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion", meta = (AllowPrivateAccess = "true"))
	float explosionRadius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion", meta = (AllowPrivateAccess = "true"))
	float explosionDamage = 150;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impulse Settings",  meta=(AllowPrivateAccess = "true"))
    float impulseStrength = 2000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impulse Settings", meta = (AllowPrivateAccess = "true"))
    float areaRadius = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scale Settings", meta = (AllowPrivateAccess = "true"))
    float minScale = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scale Settings", meta = (AllowPrivateAccess = "true"))
    float maxScale = 1.0f;

	float CalculateScaleWithMaxDistance(AMyPlayer* _player, bool bInvert);

	void DirectionProjection(AMyPlayer* _player);
	void DamageDistance(AMyPlayer* _player);

	bool CheckObstacle(AMyPlayer* _player);

public:
	void CheckOverlap();

	int playerIndex;
	int directHitPlayerIndex;

	FString teamTag;
};
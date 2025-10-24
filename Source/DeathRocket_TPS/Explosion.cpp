#include "Explosion.h"
#include "DrawDebugHelpers.h"  
#include "Kismet/GameplayStatics.h"

AExplosion::AExplosion()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AExplosion::BeginPlay()
{
    Super::BeginPlay();

}

float AExplosion::CalculateScaleWithMaxDistance(AMyPlayer* _player, bool _bInvert)
{
    FVector direction = _player->GetActorLocation() - GetActorLocation();
    direction.Normalize();
    FVector newLocation = GetActorLocation() + direction * areaRadius;
    float distance = FVector::Dist(newLocation, _player->GetActorLocation());

    float scale = 0.0f;

    if (_player->playerIndex != directHitPlayerIndex) {
        if (_bInvert)
            scale = ((distance / explosionRadius) - 1.0f) * -1.0f;
        else
            scale = distance / explosionRadius;

        if (scale < 0.5) {
            scale = minScale;
        }
    }
    else
    {
        scale = maxScale;
    }
    
    return scale;
}

void AExplosion::DirectionProjection(AMyPlayer* _player)
{
    float scale = CalculateScaleWithMaxDistance(_player, false);

    float scaleImpulseStrength = scale * impulseStrength;

    FVector direction = _player->GetActorLocation() - GetActorLocation();
    direction.Normalize();

    _player->LaunchCharacter(direction * scaleImpulseStrength, true, true);
}

void AExplosion::DamageDistance(AMyPlayer* _player)
{
    float scale = CalculateScaleWithMaxDistance(_player, false);

    float damage = scale * explosionDamage;

    _player->MyTakeDamage(damage, playerIndex);
}

bool AExplosion::CheckObstacle(AMyPlayer* _player)
{
    FVector StartLocation = GetActorLocation();
    FVector EndLocation = _player->GetActorLocation();

    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this);

    TArray<FHitResult> OutHits;

    bool bHit = GetWorld()->LineTraceMultiByChannel(OutHits, StartLocation, EndLocation, ECC_Camera, CollisionParams);

    if (bHit)
    {
        bool bPlayerHit = false;
        bool bOtherObstacles = false;

        for (const FHitResult& Hit : OutHits)
        {
            AActor* HitActor = Hit.GetActor();

            if (HitActor)
            {
                if (HitActor == _player)
                {
                    bPlayerHit = true;
                }
                else
                {
                    bOtherObstacles = true;
                }
            }
        }

        if (bPlayerHit && !bOtherObstacles)
        {
            return true;
        }
    }

    return false;
}

void AExplosion::CheckOverlap()
{
    if (ExplosionSound)
    {
        UGameplayStatics::PlaySound2D(this, ExplosionSound);
    }

    if (ExplosionParticles) {
        FVector ShotParticlesLocation = GetActorLocation();
        FRotator ShotParticlesRotation = GetActorRotation();
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticles, ShotParticlesLocation, ShotParticlesRotation, ExplosionScale);
    }

    FVector Location = GetActorLocation();
    float radius = explosionRadius;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    TArray<FHitResult> OutHits;
    bool bHit = GetWorld()->SweepMultiByChannel(OutHits, Location, Location, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(radius), QueryParams);

    if (bHit)
    {
        for (const FHitResult& Hit : OutHits)
        {
            AActor* HitActor = Hit.GetActor();

            if (HitActor && HitActor != this && !ProcessedActors.Contains(HitActor))
            {
                AMyPlayer* Player = Cast<AMyPlayer>(HitActor);

                if (Player)
                {
                    ProcessedActors.Add(HitActor); 

                    if (CheckObstacle(Player)){
                        DirectionProjection(Player);

                        if (Player->teamTag != teamTag) {
                            DamageDistance(Player); 
                        }
                    }
                }
            }
        }
    }

    Destroy();
}
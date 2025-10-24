#pragma once

#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "MyPlayer.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rocket.generated.h"

UCLASS()
class DEATHROCKET_TPS_API ARocket : public AActor
{
    GENERATED_BODY()

public:
    ARocket();

    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<AActor> ActorToSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
    int rocketDamage;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Rocket")
    USceneComponent* DefaultRootScene  = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Rocket")
    UMeshComponent* RocketMesh = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Rocket")
    UCapsuleComponent* CollisionCapsule = nullptr; 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rocket")
    float maxLifetime;

protected:
    virtual void BeginPlay() override;

private:
    void TriggerExplosion(int directHitPlayerIndex);

public:
    UFUNCTION()
    void OnLifeSpanExpired(AActor* DestroyedActor);

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    int playerIndex;

    FString teamTag;
};
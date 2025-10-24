#include "Rocket.h"
#include "Explosion.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "MyPlayer.h"
#include "Engine/World.h"
#include "TimerManager.h"

ARocket::ARocket()
{
    PrimaryActorTick.bCanEverTick = true;

    DefaultRootScene = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
    RootComponent = DefaultRootScene;

    RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RocketMesh"));
    RocketMesh->SetupAttachment(DefaultRootScene);
    
    CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision")); 
    CollisionCapsule->SetupAttachment(DefaultRootScene); 
}

void ARocket::OnLifeSpanExpired(AActor* DestroyedActor)
{
    TriggerExplosion(-1);
}

void ARocket::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void ARocket::BeginPlay()
{
    Super::BeginPlay();

    OnDestroyed.AddDynamic(this, &ARocket::OnLifeSpanExpired);

    CollisionCapsule->OnComponentBeginOverlap.AddDynamic(this, &ARocket::OnOverlapBegin);
}

void ARocket::TriggerExplosion(int directHitPlayerIndex)
{
    FVector Location = GetActorLocation();
    FRotator Rotation = FRotator::ZeroRotator;

    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ActorToSpawn, Location, Rotation);

    //Tag && Explosion
    AExplosion* explosion = Cast<AExplosion>(SpawnedActor);
    if (explosion)
    {
        explosion->playerIndex = playerIndex;
        explosion->teamTag = teamTag;
        explosion->directHitPlayerIndex = directHitPlayerIndex;
    }
    explosion->CheckOverlap(); 
}

void ARocket::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    int directHitPlayerIndex = -1;
    if (OtherActor && OtherActor != this) {
        if (Cast<AMyPlayer>(OtherActor)) {
            if (Cast<AMyPlayer>(OtherActor)->playerIndex == playerIndex) {
                return;
            }
            else {
                directHitPlayerIndex = Cast<AMyPlayer>(OtherActor)->playerIndex;
            }
        }
        Destroy();
        TriggerExplosion(directHitPlayerIndex);
    }
}
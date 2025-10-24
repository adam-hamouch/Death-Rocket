#include "Flag.h"

AFlag::AFlag()
{
	PrimaryActorTick.bCanEverTick = true;

    DefaultRootScene = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
    RootComponent = DefaultRootScene;

    sphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
    sphereCollision->SetupAttachment(RootComponent); 

    meshComponent = CreateDefaultSubobject <UStaticMeshComponent> (TEXT("Mesh")); 
    meshComponent->SetupAttachment(RootComponent); 

    sphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AFlag::OnOverlapBegin);
    sphereCollision->OnComponentEndOverlap.AddDynamic(this, &AFlag::OnOverlapEnd);
}

void AFlag::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFlag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFlag::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!bIsOnCoolDown)
    {
        if (OtherActor && OtherActor != this) {
            AMyPlayer* player = Cast<AMyPlayer>(OtherActor);
            if (player)
            {
                if (!bIsTeam1In && !bIsTeam2In)
                {
                    if (!bIsOnCoolDown)
                    {
                        FadeIn();
                    }
                }

                if (player->teamTag == "Team 1")
                {
                    bIsTeam1In = true;
                }
                else if (player->teamTag == "Team 2")
                {
                    bIsTeam2In = true;
                }

                CaptureProgress();
                Material();
            }
        }
    }
}

void AFlag::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!bIsOnCoolDown)
    {
        if (OtherActor && OtherActor != this) {
            AMyPlayer* player = Cast<AMyPlayer>(OtherActor);
            if (player) 
            {
                bool bIsTeam1StillIn = false;
                bool bIsTeam2StillIn = false;

                CheckTeam(bIsTeam1StillIn, bIsTeam2StillIn);

                bIsTeam1In = bIsTeam1StillIn;
                bIsTeam2In = bIsTeam2StillIn;

                CaptureProgress();
                Material();

                if (!bIsTeam1In && !bIsTeam2In)
                {
                    if (!bIsOnCoolDown)
                    {
                        FadeOut();
                    }
                }
            }
        }
    }
}

void AFlag::CheckTeam(bool& _bIsTeam1StillIn, bool& _bIsTeam2StillIn)
{
    TArray<AActor*> OverlappingActors;
    sphereCollision->GetOverlappingActors(OverlappingActors, AMyPlayer::StaticClass());

    for (AActor* Actor : OverlappingActors) {
        AMyPlayer* player = Cast<AMyPlayer>(Actor);
        if (player)
        {
            if (player->teamTag == "Team 1")
            {
                _bIsTeam1StillIn = true;
            }
            else if (player->teamTag == "Team 2")
            {
                _bIsTeam2StillIn = true;
            }
        }
    }
}

void AFlag::RefillUltimateTeam1()
{
    TArray<AActor*> OverlappingActors;
    sphereCollision->GetOverlappingActors(OverlappingActors, AMyPlayer::StaticClass());

    for (AActor* Actor : OverlappingActors) {
        AMyPlayer* player = Cast<AMyPlayer>(Actor);
        if (player)
        {
            if (player->teamTag == "Team 1")
            {
                player->ChargeUltimate(ultimeValue);
            }  
        }
    }
}

void AFlag::RefillUltimateTeam2()
{
    TArray<AActor*> OverlappingActors;
    sphereCollision->GetOverlappingActors(OverlappingActors, AMyPlayer::StaticClass());

    for (AActor* Actor : OverlappingActors) {
        AMyPlayer* player = Cast<AMyPlayer>(Actor);
        if (player)
        {
            if (player->teamTag == "Team 2")
            {
				player->ChargeUltimate(ultimeValue);
            }
        }
    }
}

void AFlag::Reset()
{
	// Reset the flag
    bIsOnCoolDown = true;
    crtPrgValTeam1 = 0.0f; 
    crtPrgValTeam2 = 0.0f; 
	bIsTeam1In = false;
	bIsTeam2In = false;
    meshComponent->SetMaterial(0, m_white); 

	// HUD update
    FadeOut();
    UpdateWarningMessage(); 

	// Clear the timer and set a new one for the cooldown
	GetWorld()->GetTimerManager().ClearTimer(FlagTimerHandle);  
    GetWorld()->GetTimerManager().ClearTimer(CoolDownHandle);
    GetWorld()->GetTimerManager().SetTimer(CoolDownHandle, this, &AFlag::CoolDown, CoolDownTimer, false);

	// Broadcast the new values
	OnPrgValTeam2ChangedDelegate.Broadcast(crtPrgValTeam2);
	OnPrgValTeam1ChangedDelegate.Broadcast(crtPrgValTeam1);
}

void AFlag::CaptureProgress()
{
    if (bIsTeam1In && bIsTeam2In) {
        GetWorld()->GetTimerManager().ClearTimer(FlagTimerHandle);
    }
    else
    {
        GetWorld()->GetTimerManager().ClearTimer(FlagTimerHandle);

        if (bIsTeam1In) {
            GetWorld()->GetTimerManager().SetTimer(FlagTimerHandle, [this]()
                {
                    Team1(updateInterval);
                }, updateInterval, true);
        }
        else if (bIsTeam2In) {
            GetWorld()->GetTimerManager().SetTimer(FlagTimerHandle, [this]()
                {
                    Team2(updateInterval);
                }, updateInterval, true);
        }
    }

    if (!bIsTeam1In && !bIsTeam2In)
    {
        GetWorld()->GetTimerManager().ClearTimer(FlagTimerHandle);
    }
}

void AFlag::Team1(float _deltaTime)
{
    float adjustedVal = icrVal * _deltaTime;

    if (crtPrgValTeam1 < crtPrgValTeam2) {
        crtPrgValTeam2 -= adjustedVal;
        crtPrgValTeam2 = FMath::Clamp(crtPrgValTeam2, 0.0f, maxPrgVal); 
        OnPrgValTeam2ChangedDelegate.Broadcast(crtPrgValTeam2);
    }
    else {
        crtPrgValTeam1 += adjustedVal;
        crtPrgValTeam1 = FMath::Clamp(crtPrgValTeam1, 0.0f, maxPrgVal); 
        OnPrgValTeam1ChangedDelegate.Broadcast(crtPrgValTeam1);
    }

    if (crtPrgValTeam1 >= maxPrgVal) {
        RefillUltimateTeam1();
        Reset();
    }
}

void AFlag::Team2(float _deltaTime)
{
    float adjustedVal = icrVal * _deltaTime;

    if (crtPrgValTeam2 < crtPrgValTeam1) {
        crtPrgValTeam1 -= adjustedVal;
        crtPrgValTeam1 = FMath::Clamp(crtPrgValTeam1, 0.0f, maxPrgVal); 
        OnPrgValTeam1ChangedDelegate.Broadcast(crtPrgValTeam1);
    }
    else {
        crtPrgValTeam2 += adjustedVal;
        crtPrgValTeam2 = FMath::Clamp(crtPrgValTeam2, 0.0f, maxPrgVal);
        OnPrgValTeam2ChangedDelegate.Broadcast(crtPrgValTeam2);
    }

    if (crtPrgValTeam2 >= maxPrgVal) {
        RefillUltimateTeam2();
        Reset();
    }
}

void AFlag::CoolDown()
{
	// Cancel the flag
    bIsOnCoolDown = false; 

	// HUD update
	UpdateWarningMessage();
}

void AFlag::Material()
{
    if (bIsTeam1In && bIsTeam2In) {
        meshComponent->SetMaterial(0, m_yellow); 
        BothTeamOnPoint(); 
    }
    else
    {
        if (bIsTeam1In) {
            meshComponent->SetMaterial(0, m_red);
        }
        if (bIsTeam2In) {
            meshComponent->SetMaterial(0, m_blue);
        }
    }

    if (!bIsTeam1In && !bIsTeam2In)
    {
        meshComponent->SetMaterial(0, m_white); ;
    }
}

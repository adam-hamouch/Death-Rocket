// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayer.h"
#include "Rocket.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include "Public/TeamsManager.h"
#include "Kismet/GameplayStatics.h"

#define DEBUGMESSAGE(x, ...) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT(x), __VA_ARGS__));}

// Sets default values
AMyPlayer::AMyPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// SpringArm
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Player SpringArm"));
	springArm->SetupAttachment(RootComponent);
	springArm->TargetArmLength = 400.f;
	springArm->bUsePawnControlRotation = true;
	springArm->CameraLagSpeed = 3.f;

	// Camera
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Camera"));
	camera->SetupAttachment(springArm, USpringArmComponent::SocketName);
	camera->bUsePawnControlRotation = false;

	// Niagara
	niagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraSystem"));
	niagaraComponent->SetupAttachment(RootComponent);

	//ulti = NewObject<UPlayerUltiInfiniteRocket>();
}

// Called when the game starts or when spawned
void AMyPlayer::BeginPlay()
{
	Super::BeginPlay();

	// Health / Shield
	crtHealth = maxHealth;
	crtShield = maxShield;

	// Attack
	nbRocketLoader = nbMaxRocketLoader;
	crtUltiBarValue = maxUtliBarValue;
	
	// Niagara
	DisableUltimEffect();

	InitialMeshTransform = GetMesh()->GetRelativeTransform();

	this->GetCharacterMovement()->MaxWalkSpeed = runSpeed;
	this->GetCharacterMovement()->MaxWalkSpeedCrouched = crouchSpeed;
}

// Called every frame
void AMyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDied)	
	{
		timerBeforeRespawn -= DeltaTime;

		if (timerBeforeRespawn <= 0.f) {
			Respawn();

			isInvincible = true;
		}
	}

	if (isInvincible) {
		timerBeforeVulnerable -= DeltaTime;

		if (timerBeforeVulnerable <= 0.f) {
			isInvincible = false;
		}
	}

	timeSinceLastDamage += DeltaTime;

	UpdateGrappling(DeltaTime);

	ShieldRegeneration(DeltaTime);
	UpdateTimeBetweenShoot(DeltaTime);
	UpdateRocketReload(DeltaTime);

	if (ulti)
	{
		if (bIsUltiActivate)
		{
			ulti->UpdateUlti(this, DeltaTime);
		}
		else if (ulti->bIsToDelete)
		{
			ulti = nullptr;
			DisableUltimEffect();
		}
	}

	UpdateFOVTransition(DeltaTime);
}

// Called to bind functionality to input
void AMyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			Subsystem->AddMappingContext(MappingContext, 0);
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPlayer::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyPlayer::Look);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyPlayer::StartJump);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMyPlayer::StopJump);

		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AMyPlayer::StartCrouching);

		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AMyPlayer::StopCrouching);

		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AMyPlayer::StartSprinting);

		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMyPlayer::StopSprinting);

		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AMyPlayer::StartAiming);

		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AMyPlayer::StopAiming);

		EnhancedInputComponent->BindAction(ShootRocketAction, ETriggerEvent::Started, this, &AMyPlayer::ShootRocket);

		EnhancedInputComponent->BindAction(ReloadRocketAction, ETriggerEvent::Started, this, &AMyPlayer::StartRocketReloadWithInput);

		EnhancedInputComponent->BindAction(StartUltiAction, ETriggerEvent::Started, this, &AMyPlayer::StartUlti);

		EnhancedInputComponent->BindAction(GrapplingHookAction, ETriggerEvent::Started, this, &AMyPlayer::StartGrappling);

		EnhancedInputComponent->BindAction(GrapplingHookAction, ETriggerEvent::Completed, this, &AMyPlayer::StopGrappling);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("can't find the MappingContext"));
	}
}

int AMyPlayer::GetNbRocketLoader() const
{
	return nbRocketLoader;
}

void AMyPlayer::Move(const FInputActionValue& _IAValue)
{
	if (!Controller)
		return;

	AddMovementInput(GetActorForwardVector(), _IAValue.Get<FVector>().Y, false);
	AddMovementInput(GetActorRightVector(), _IAValue.Get<FVector>().X, false);
}

void AMyPlayer::Look(const FInputActionValue& _IAValue)
{
	if (!Controller)
		return;

	FVector2D sensi = lookSensibility;
	if (Aiming)
		sensi = aimLookSensibility;

	AddControllerYawInput(_IAValue.Get<FVector>().X * sensi.X);
	AddControllerPitchInput(-_IAValue.Get<FVector>().Y * sensi.Y);
}

void AMyPlayer::StartCrouching(const FInputActionValue& _IAValue)
{
	if (!(GetCharacterMovement()->IsFalling()))
	{
		if (Sprinting)
			Sprinting = false;
		Crouch();
	}
}

void AMyPlayer::StopCrouching(const FInputActionValue& _IAValue)
{
	UnCrouch();
}

void AMyPlayer::StartJump(const FInputActionValue& _IAValue)
{
	Jump();
}

void AMyPlayer::StopJump(const FInputActionValue& _IAValue)
{
	StopJumping();
}

void AMyPlayer::StartSprinting(const FInputActionValue& _IAValue)
{
	if (!Controller)
		return;

	if (!(GetCharacterMovement()->IsFalling()))
	{
		if (Aiming)
		{
			Aiming = false;
			GetCharacterMovement()->MaxWalkSpeedCrouched = crouchSpeed;
		}

		if (GetCharacterMovement()->IsCrouching())
			UnCrouch();

		ChangeFOV(sprintFOV, timeFOVTransitionSprint);
		Sprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = sprintSpeed;
	}
}

void AMyPlayer::StopSprinting(const FInputActionValue& _IAValue)
{
	if (!Controller)
		return;

	if (Sprinting)
	{
		Sprinting = false;
		ChangeFOV(initalFOV, timeFOVTransitionSprint);
		GetCharacterMovement()->MaxWalkSpeed = runSpeed;
	}
}

void AMyPlayer::StartAiming(const FInputActionValue& _IAValue)
{
	if (!Controller)
		return;

	if (Sprinting)
		Sprinting = false;

	ChangeFOV(aimFOV, timeFOVTransitionAim);

	GetCharacterMovement()->MaxWalkSpeedCrouched = aimCrouchSpeed;
	GetCharacterMovement()->MaxWalkSpeed = aimWalkSpeed;

	Aiming = true;
}

void AMyPlayer::StopAiming(const FInputActionValue& _IAValue)
{
	if (!Controller)
		return;

	if (Aiming)
	{
		ChangeFOV(initalFOV, timeFOVTransitionAim);

		GetCharacterMovement()->MaxWalkSpeedCrouched = crouchSpeed;
		GetCharacterMovement()->MaxWalkSpeed = runSpeed;
		Aiming = false;
	}
}

void AMyPlayer::StartGrappling(const FInputActionValue& _IAValue)
{
	if (!bCanGrappling)
		return;

	bIsGrappling = true;

	FVector StartLocation = GetActorLocation() + (GetActorForwardVector() * rocketSpawnOffset.X) + GetActorRightVector() * rocketSpawnOffset.Y + FVector{ 0.f, 0.f, rocketSpawnOffset.Z };
	FVector orientation = GetActorLocation() - camera->GetSocketLocation(USpringArmComponent::SocketName) + springArm->SocketOffset;
	FVector EndLocation = StartLocation + orientation * grapplingHookRange;

	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(this);

	ECollisionChannel collisionChannel = ECC_Camera;

	TArray<FHitResult> outHits;
	bool bHit = GetWorld()->LineTraceMultiByChannel(outHits, StartLocation, EndLocation, collisionChannel, collisionParams);
	
	if (bHit)
	{
		for (FHitResult& hit : outHits)
		{
			AActor* HitActor = hit.GetActor();
			if (HitActor)
			{
				grapplingDirectionVec = hit.ImpactPoint - GetActorLocation();
				grapplingDirectionVec.Normalize();
				grapplingHitPoint = hit.ImpactPoint;
				bCanGrappling = false;
			}
		}
	}
}

void AMyPlayer::UpdateGrappling(float _deltaTime)
{
	if (!bCanGrappling && !GetCharacterMovement()->IsFalling())
		bCanGrappling = true;


	if (!bIsGrappling)
		return;

	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(this);
	ECollisionChannel collisionChannel = ECC_Camera;
	TArray<FHitResult> outHits;
	FVector startLocation = GetActorLocation();
	FVector endLocation = GetActorLocation() + grapplingDirectionVec * (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 1.25f);

	bool bHit = GetWorld()->LineTraceMultiByChannel(outHits, startLocation, endLocation, collisionChannel, collisionParams);


	if (bHit)
	{
		FInputActionValue iaAction{};
		StopGrappling(iaAction);
		return;
	}

	DrawGrapplingCable();
	LaunchCharacter(grapplingDirectionVec * grapplingSpeed, true, true);
}

void AMyPlayer::StopGrappling(const FInputActionValue& _IAValue)
{
	bIsGrappling = false;
	bCanGrappling = false;
	grapplingDirectionVec = FVector::ZeroVector;
	grapplingHitPoint = FVector::ZeroVector;
	HideGrapplingCable();
}

void AMyPlayer::ShootRocket()
{
	if (!rocketObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy type is null!"));
		return;
	}

	if (bIsReloading || !bCanShoot)
		return;

	if (ShootSound)
	{
		UGameplayStatics::PlaySound2D(this, ShootSound);
	}

	if (GetWorld())
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			if (GetCharacterMovement()->IsCrouching())
				AnimInstance->Montage_Play(CrouchShootAnim);
			else
				AnimInstance->Montage_Play(NormalShootAnim);
		}
		FVector spawnPos = GetActorLocation() + (GetActorForwardVector() * rocketSpawnOffset.X) + GetActorRightVector() * rocketSpawnOffset.Y + FVector{ 0.f, 0.f, rocketSpawnOffset.Z };
		FVector orientation = GetActorLocation() - camera->GetSocketLocation(USpringArmComponent::SocketName) + springArm->SocketOffset;

		AActor* SpawnedEnemy = GetWorld()->SpawnActor<AActor>(rocketObject, spawnPos, orientation.Rotation());
		if (!SpawnedEnemy)
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn enemy"));

		ARocket* rocket = Cast<ARocket>(SpawnedEnemy);

		if (ShotParticles)
		{
			FVector ShotParticlesLocation = GetMesh()->GetSocketLocation(TEXT("ShootingSocket"));
			FRotator ShotParticlesRotation = GetMesh()->GetSocketRotation(TEXT("ShootingSocket"));
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShotParticles, ShotParticlesLocation, ShotParticlesRotation);
		}

		if (rocket)
		{
			rocket->playerIndex = playerIndex;
			rocket->teamTag = teamTag;
		}

		bCanShoot = false;
		timerBetweenShoot = timeBetweenShoot;
		--nbRocketLoader;
		if (nbRocketLoader <= 0)
			StartRocketReload();

		OnRocketChangedDelegate.Broadcast(nbRocketLoader);
	}
}

void AMyPlayer::StartUlti(const FInputActionValue& _IAValue)
{
	if (!ulti)
		return;

	if (crtUltiBarValue >= maxUtliBarValue)
	{
		EnableUltimEffect();

		bIsUltiActivate = true;
		ulti->StartUlti(this);
		crtUltiBarValue = 0.f;

		OnUltimeChangedDelegate.Broadcast(crtUltiBarValue);
	}
}

void AMyPlayer::MyTakeDamage(const float _damage, int shooterId)
{
	timeSinceLastDamage = 0.f;

	if (!isInvincible) 
	{
		if (crtShield > 0.f)
		{
			crtShield -= _damage;
			OnShieldChangedDelegate.Broadcast(crtShield);

			if (crtShield < 0.f)
			{
				crtHealth += crtShield;
				crtShield = 0.f;
				OnHealthChangedDelegate.Broadcast(crtHealth);
			}
		}
		else
		{
			crtHealth -= _damage;
			OnHealthChangedDelegate.Broadcast(crtHealth);
		}

		if (crtHealth <= 0.f)
		{
			if (teamsManager)
			{
				teamsManager->players[shooterId]->playerScore += 200;
			}
			OnScoreBoardChangedDelegate.Broadcast();
			Die();
		}
	}
}

void AMyPlayer::Respawn()
{
	bIsDied = false;
	crtHealth = maxHealth;
	crtShield = maxShield;
	crtUltiBarValue = maxUtliBarValue;
	ulti = nullptr;
	timeSinceLastDamage = 0.f;
	nbRocketLoader = nbMaxRocketLoader;
	timerBetweenShoot = 0.f;

	bIsUltiActivate = false;
	bIsChangingFov = false;
	bIsReloading = false;
	bCanShoot = true;

	if (GetCharacterMovement()->IsCrouching())
		UnCrouch();

	GetMovementComponent()->Velocity = FVector::ZeroVector;
	camera->SetFieldOfView(initalFOV);
	SetActorLocation(respawnPos);
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetRelativeTransform(InitialMeshTransform);

	ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Cast<APlayerController>(GetController())->GetLocalPlayer())->AddMappingContext(MappingContext, 0);

	OnHealthChangedDelegate.Broadcast(maxHealth);
	OnShieldChangedDelegate.Broadcast(maxShield);
}

int AMyPlayer::GetNbMaxRocketLoader() const
{
	return nbMaxRocketLoader;
}

void AMyPlayer::SetNbRocketLoader(const int _nbRocket)
{
	nbRocketLoader = _nbRocket;
}

void AMyPlayer::SetIsReloading(const bool _isReloading)
{
	bIsReloading = _isReloading;
}

void AMyPlayer::ChargeUltimate(float _ultiValue)
{
	crtUltiBarValue += _ultiValue;
	if (crtUltiBarValue >= maxUtliBarValue)
		crtUltiBarValue = maxUtliBarValue;
	OnUltimeChangedDelegate.Broadcast(crtUltiBarValue);
}

void AMyPlayer::RegenerateShield(float _addShieldValue)
{
	crtShield += _addShieldValue;
	if (crtShield >= maxShield)
		crtShield = maxShield;
	OnShieldChangedDelegate.Broadcast(crtShield);
}

void AMyPlayer::RegenerateHealth(float _addHealthValue)
{
	crtHealth += _addHealthValue;
	if (crtHealth >= maxHealth)
		crtHealth = maxHealth;
	OnHealthChangedDelegate.Broadcast(crtHealth);
}

void AMyPlayer::ResetNbRocket()
{
	if (nbRocketLoader != nbMaxRocketLoader)
		nbRocketLoader = nbMaxRocketLoader;
	OnRocketChangedDelegate.Broadcast(nbRocketLoader);
}

void AMyPlayer::DisableUlti()
{
	bIsUltiActivate = false;
}

void AMyPlayer::ShieldRegeneration(float _deltaTime)
{
	if (crtShield < maxShield && timeSinceLastDamage >= timeBeforeShieldRegeneration)
	{
		crtShield += shieldRegenerationSpeed * _deltaTime;
		if (crtShield > maxShield)
			crtShield = maxShield;

		OnShieldChangedDelegate.Broadcast(crtShield);
	}
}

void AMyPlayer::StartRocketReload()
{
	if (nbRocketLoader == nbMaxRocketLoader)
		return;

	bIsReloading = true;
	timerReload = timeReload;
}

void AMyPlayer::StartRocketReloadWithInput(const FInputActionValue& _IAValue)
{
	if (nbRocketLoader == nbMaxRocketLoader || !Controller)
		return;

	bIsReloading = true;
	timerReload = timeReload;
}

void AMyPlayer::UpdateTimeBetweenShoot(float _deltaTime)
{
	if (bCanShoot)
		return;

	timerBetweenShoot -= _deltaTime;
	if (timerBetweenShoot <= 0.f)
		bCanShoot = true;
}

void AMyPlayer::UpdateRocketReload(float _deltaTime)
{
	if (!bIsReloading)
		return;

	timerReload -= _deltaTime;
	if (timerReload <= 0.f)
	{
		bIsReloading = false;
		nbRocketLoader = nbMaxRocketLoader;
		OnRocketChangedDelegate.Broadcast(nbRocketLoader);
	}
}

void AMyPlayer::ChangeFOV(float _nextFOV, float _timeTransition)
{
	bIsChangingFov = true;
	startTargetFOV = camera->FieldOfView;
	finalTargetFOV = _nextFOV;
	timerTargetFOV = _timeTransition;
	timerMaxTargetFOV = _timeTransition;
}

void AMyPlayer::UpdateFOVTransition(float _deltaTime)
{
	if (!bIsChangingFov)
		return;

	timerTargetFOV -= _deltaTime;
	float ratio = 1.f - (timerTargetFOV / timerMaxTargetFOV);
	camera->FieldOfView = FMath::Lerp(startTargetFOV, finalTargetFOV, ratio);

	if (ratio > 1.f)
	{
		camera->FieldOfView = finalTargetFOV;
		bIsChangingFov = false;
	}
}

void AMyPlayer::Die()
{
	ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Cast<APlayerController>(GetController())->GetLocalPlayer())->RemoveMappingContext(MappingContext);
	GetMesh()->SetSimulatePhysics(true);
	bIsDied = true;
	timerBeforeRespawn = timeBeforeRespawn;
	timerBeforeVulnerable = timeBeforeVulnerable; 
}

void AMyPlayer::ChooseUlti()
{
	switch (ultiId)
	{
	case 0 :
		ulti = NewObject<UPlayerUltiInfiniteRocket>();
		break;
	case 1 :
		ulti = NewObject<UPlayerUltiIncreaseShield>();
		break;
	case 2 :
		ulti = NewObject<UPlayerUltiIncreaseHealth>();
		break;
	}
}
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Public/PlayerUltis.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "MyPlayer.generated.h"

class UTeamsManager;
class ARocket;
class UInputMappingContext;
class UInputAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShieldChangedSignature, float, newShield);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature, float, newHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUltimeChangedSignature, float, newUltime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRocketChangedSignature, int, newRocket);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScoreBoardChangedSignature);


UCLASS()
class DEATHROCKET_TPS_API AMyPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyPlayer();

#pragma region Teams infos
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Personall, meta = (AllowPrivateAccess = "true"))
	int playerIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* ShootSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Personall, meta = (AllowPrivateAccess = "true"))
	FString teamTag = "Team 1";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Personall, meta = (AllowPrivateAccess = "true"))
	int playerScore = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Personall, meta = (AllowPrivateAccess = "true"))
	bool isInvincible = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Personall, meta = (AllowPrivateAccess = "true"))
	UTeamsManager* teamsManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Personall, meta = (AllowPrivateAccess = "true"))
	int ultiId;
#pragma endregion

#pragma region Hud Infos
	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnShieldChangedSignature OnShieldChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnHealthChangedSignature OnHealthChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnRocketChangedSignature OnRocketChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnUltimeChangedSignature OnUltimeChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnScoreBoardChangedSignature OnScoreBoardChangedDelegate;

	FVector respawnPos;
#pragma endregion

protected:
#pragma region Player Controls
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* MappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ShootRocketAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadRocketAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* StartUltiAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* GrapplingHookAction;
#pragma endregion

#pragma region Player Components
	UPROPERTY(EditAnywhere)
	class UCameraComponent* camera;
	UPROPERTY(EditAnywhere)
	class USpringArmComponent* springArm;
#pragma endregion

#pragma region Camera
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	FVector2D lookSensibility = { 1.f, 1.f };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	FVector2D aimLookSensibility = { 0.7f, 0.7f };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float initalFOV = 90.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float sprintFOV = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float timeFOVTransitionSprint = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float aimFOV = 60.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float timeFOVTransitionAim = 0.15f;
#pragma endregion

#pragma region Player Health
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
	float maxHealth = 100.f;
#pragma endregion

#pragma region Player Respawn
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Respawn")
	float timeBeforeRespawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Respawn")
	float timeBeforeVulnerable;
#pragma endregion

#pragma region Player Shield
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shield")
	float maxShield = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shield")
	float shieldRegenerationSpeed = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shield")
	float timeBeforeShieldRegeneration = 5.f;
#pragma endregion

#pragma region Crouch Aim and Sprint
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Default)
	bool Sprinting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Default)
	bool Aiming;

	UPROPERTY(EditAnywhere, Category = Default)
	float sprintSpeed;

	UPROPERTY(EditAnywhere, Category = Default)
	float runSpeed;

	UPROPERTY(EditAnywhere, Category = Default)
	float aimWalkSpeed;

	UPROPERTY(EditAnywhere, Category = Default)
	float crouchSpeed;

	UPROPERTY(EditAnywhere, Category = Default)
	float aimCrouchSpeed;
#pragma endregion

#pragma region GrapplingHook
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grappling")
	float grapplingSpeed = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grappling")
	float grapplingHookRange = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grappling")
	FVector grapplingHitPoint = FVector::ZeroVector;
#pragma endregion

#pragma region Player Attack
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	int nbMaxRocketLoader = 2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float timeBetweenShoot = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float timeReload = 2.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	TSubclassOf<AActor> rocketObject;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	FVector rocketSpawnOffset;
#pragma endregion

#pragma region Player Ulti Attack
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ulti")
	float maxUtliBarValue = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ulti")
	float reloadUltiBarSpeed = 25.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ulti")
	UNiagaraComponent* niagaraComponent;
	
#pragma endregion

#pragma region Player Animations
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animations")
	UAnimMontage* JumpAnim;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animations")
	UAnimMontage* LandAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animations")
	UAnimMontage* NormalShootAnim;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animations")
	UAnimMontage* CrouchShootAnim;
#pragma endregion

#pragma region Particles
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Particles")
	UParticleSystem* ShotParticles = nullptr;
#pragma endregion

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	int GetNbRocketLoader() const;

	UFUNCTION()
	void Move(const FInputActionValue& _IAValue);
	UFUNCTION()
	void Look(const FInputActionValue& _IAValue);
	
	UFUNCTION()
	void StartCrouching(const FInputActionValue& _IAValue);
	UFUNCTION()
	void StopCrouching(const FInputActionValue& _IAValue);

	UFUNCTION()
	void StartSprinting(const FInputActionValue& _IAValue);
	UFUNCTION()
	void StopSprinting(const FInputActionValue& _IAValue);

	UFUNCTION()
	void StartJump(const FInputActionValue& _IAValue);
	UFUNCTION()
	void StopJump(const FInputActionValue& _IAValue);

	UFUNCTION()
	void StartAiming(const FInputActionValue& _IAValue);
	UFUNCTION()
	void StopAiming(const FInputActionValue& _IAValue);

	UFUNCTION()
	void StartGrappling(const FInputActionValue& _IAValue);
	void UpdateGrappling(float _deltaTime);
	UFUNCTION()
	void StopGrappling(const FInputActionValue& _IAValue);
	UFUNCTION(BlueprintImplementableEvent)
	void DrawGrapplingCable();
	UFUNCTION(BlueprintImplementableEvent)
	void HideGrapplingCable();

	UFUNCTION()
	void ShootRocket();
	UFUNCTION()
	void StartUlti(const FInputActionValue& _IAValue);

	UFUNCTION(BlueprintCallable)
	void MyTakeDamage(const float _damage, int shooterId);

	UFUNCTION(BlueprintCallable)
	void ChooseUlti();

	UFUNCTION(BlueprintCallable)
	void Respawn();

	UFUNCTION(BlueprintImplementableEvent)
	void AddHUDSelect();
	UFUNCTION(BlueprintImplementableEvent)
	void EnableUltimEffect();
	UFUNCTION(BlueprintImplementableEvent)
	void DisableUltimEffect();

	int GetNbMaxRocketLoader() const;
	void SetNbRocketLoader(const int _nbRocket);
	void SetIsReloading(const bool _isReloading);

	void ChargeUltimate(float _ultiValue);

	void RegenerateShield(float _addShieldValue);
	void RegenerateHealth(float _addHealthValue);
	void ResetNbRocket();
	void DisableUlti();

	UFUNCTION(BlueprintCallable)
	void Die();
private:
	FTransform InitialMeshTransform = FTransform::Identity;
	FVector grapplingDirectionVec = FVector::ZeroVector;
	UIPlayerUlti* ulti = nullptr;
	float crtHealth = 0.f;
	float crtShield = 0.f;
	float crtUltiBarValue = 0.f;
	float timeSinceLastDamage = 0.f;
	float timerBeforeRespawn = 0.f;
	float timerBeforeVulnerable = 0.f;
	int nbRocketLoader = 0;
	float timerBetweenShoot = 0.f;
	float timerReload = 0.f;
	float startTargetFOV = 0.f;
	float finalTargetFOV = 0.f;
	float timerTargetFOV = 0.f;
	float timerMaxTargetFOV = 0.f;
	bool bIsGrappling = false;
	bool bCanGrappling = false;
	bool bIsUltiActivate = false;
	bool bIsChangingFov = false;
	bool bIsReloading = false;
	bool bCanShoot = true;
	bool bIsDied = false;

	void ShieldRegeneration(float _deltaTime);
	void StartRocketReload();
	void StartRocketReloadWithInput(const FInputActionValue& _IAValue);
	void UpdateTimeBetweenShoot(float _deltaTime);
	void UpdateRocketReload(float _deltaTime);
	void ChangeFOV(float _nextFOV, float _timeTransition);
	void UpdateFOVTransition(float _deltaTime);
};
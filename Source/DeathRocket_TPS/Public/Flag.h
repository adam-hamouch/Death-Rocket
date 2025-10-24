#pragma once

#include "../MyPlayer.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Flag.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPrgValTeam2ChangedSignature, float, newPrgValTeam2);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPrgValTeam1ChangedSignature, float, newPrgValTeam1);

UCLASS()
class DEATHROCKET_TPS_API AFlag : public AActor
{
	GENERATED_BODY()
	
public:	
	AFlag();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Flag")
	USceneComponent* DefaultRootScene = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Flag")
	USphereComponent* sphereCollision = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Flag")
	UStaticMeshComponent* meshComponent;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void FadeOut();

	UFUNCTION(BlueprintImplementableEvent)
	void FadeIn();

	UFUNCTION(BlueprintImplementableEvent)
	void BothTeamOnPoint(); 

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateWarningMessage();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterial* m_blue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterial* m_yellow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterial* m_red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterial* m_white;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnPrgValTeam2ChangedSignature OnPrgValTeam2ChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnPrgValTeam1ChangedSignature OnPrgValTeam1ChangedDelegate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Progression")
	float maxPrgVal = 10.0f; 

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flag parameters")
	float CoolDownTimer = 120.0f; 

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flag parameters")
	float ultimeValue = 50.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Flag parameters")
	bool bIsOnCoolDown = false; 

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float crtPrgValTeam1 = 0.0f;
	float crtPrgValTeam2 = 0.0f; 

	float icrVal = 1.0f;
	float updateInterval = 0.001f;

	bool bIsTeam1In = false;
	bool bIsTeam2In = false;

	void RefillUltimateTeam1();
	void RefillUltimateTeam2(); 

	void Reset(); 

	void CheckTeam(bool& _bIsTeam1StillIn, bool& _bIsTeam2StillIn);
	void CaptureProgress(); 

	void Team1(float _deltaTime);
	void Team2(float _deltaTime);

	void CoolDown(); 

	void Material(); 

	FTimerHandle FlagTimerHandle;
	FTimerHandle CoolDownHandle; 
};

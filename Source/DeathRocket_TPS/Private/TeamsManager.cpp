#include "TeamsManager.h"
#include "../MyPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "GameModeNew.h"
#include "EngineUtils.h"
#include "Components/TextRenderComponent.h"

void UTeamsManager::Init1vs1()
{
	CreatePlayers(2);
	SetPseudos(1);
}

void UTeamsManager::Init2vs2()
{
	CreatePlayers(4);
	SetPseudos(3);
}

void UTeamsManager::CreatePlayers(int nums)
{
	FString stringError = "Error";
	FString teamTag = "null";
	
	for (int i = 1; i < nums; ++i)
	{
		ULocalPlayer* LocalPlayer = GetWorld()->GetGameInstance()->CreateLocalPlayer((int32)i, stringError, true);

		switch (nums)
		{
		case 2:
			teamTag = "Team 2";
			break;
		case 4:
			if (i == 1)
			{
				teamTag = "Team 1";
			}
			else
			{
				teamTag = "Team 2";
			}
			break;
		}

		if (LocalPlayer)
		{
			AMyPlayer* player = Cast<AMyPlayer>(LocalPlayer->PlayerController->GetPawn());

			if (player)
			{
				player->playerIndex = i;
				player->teamTag = teamTag;
				player->respawnPos = player->GetActorLocation();
			}
		}
	}

	TArray<AMyPlayer*> playersRef;
	GetAllActorsOfType<AMyPlayer>(GetWorld(), playersRef);

	for (AMyPlayer* player : playersRef)
	{
		if (player->playerIndex == 0)
			player->respawnPos = firstPlayerPos;

		if (UTextRenderComponent* TextComponent = player->FindComponentByClass<UTextRenderComponent>())
		{
			FString PlayerText = FString::Printf(TEXT("Player %d"), player->playerIndex + 1);
			TextComponent->SetText(FText::FromString(PlayerText));
		}

		player->AddHUDSelect();
		players.Add(player);
	}
}

void UTeamsManager::EndGame()
{
	AGameModeNew* gameMode = Cast<AGameModeNew>(UGameplayStatics::GetGameMode(GetWorld()));

	for (AMyPlayer* player : players)
	{
		if (player->teamTag == "Team 1")
			scoreTeam1 += player->playerScore;
		else if (player->teamTag == "Team 2")
			scoreTeam2 += player->playerScore;
	}

	if (scoreTeam1 > scoreTeam2)
		gameMode->FirstTeamWin();
	else if (scoreTeam2 > scoreTeam1)
		gameMode->SecondTeamWin();
	else
		gameMode->Draw();

	DestroyPlayers();
}

void UTeamsManager::DestroyPlayers()
{
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (!GameInstance)
		return;

	for (AMyPlayer* player : players)
	{
		APlayerController* playerController = Cast<APlayerController>(player->GetController());
		if (player->playerIndex != 0)
			playerController->Destroy();

		player->Destroy();
	}

	TArray<ULocalPlayer*> LocalPlayers = GameInstance->GetLocalPlayers();

	for (int32 i = LocalPlayers.Num() - 1; i >= 1; --i)
	{
		ULocalPlayer* LocalPlayer = LocalPlayers[i];
		if (LocalPlayer)
		{
			GameInstance->RemoveLocalPlayer(LocalPlayer);
		}
	}
	
	players.Empty();
}

template<typename T>
inline void GetAllActorsOfType(UWorld* World, TArray<T*>& OutActors)
{
	if (!World) return;

	for (TActorIterator<T> It(World); It; ++It)
	{
		OutActors.Add(*It);
	}
}
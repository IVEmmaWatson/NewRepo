// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ZeusGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ZEUS_API AZeusGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	virtual void PlayerEliminated(class AZeusCharacter* EliminatedCharacter, class AZeusPlayerController* VictimController, AZeusPlayerController* AttackController);

	virtual void RequestRespawn(class ACharacter* ELimmedCharacter, AController* ElimmedController);;
};

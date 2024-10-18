// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ZeusGameMode.generated.h"


namespace MatchState
{
	extern ZEUS_API const FName Cooldown;	// ������ʱ���ﵽʱ����ʾ��ʤ�߲���ʼ��ȴʱ��
}


/**
 * 
 */
UCLASS()
class ZEUS_API AZeusGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	// ���� AZeusGameMode ���ʵ��ʱ��������캯���ᱻ���á�
	AZeusGameMode();
	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(class AZeusCharacter* EliminatedCharacter, class AZeusPlayerController* VictimController, AZeusPlayerController* AttackController);

	virtual void RequestRespawn(class ACharacter* ELimmedCharacter, AController* ElimmedController);;

	// ����ʱ��
	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

	float LevelStartingTime = 0.f;
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;
private:
	float CountdownTime = 0.f;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ZeusPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class ZEUS_API AZeusPlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:

private:
	class AZeusCharacter* Character;
	class AZeusPlayerController* PlayerController;


public:
	// ���ظ����RPC��������score�����仯ʱ���Զ�����
	virtual void OnRep_Score() override;
	// ����score
	void AddToScore(float ScoreAmount);
};

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
	// 重载父类的RPC函数，当score发生变化时会自动调用
	virtual void OnRep_Score() override;
	// 设置score
	void AddToScore(float ScoreAmount);
};

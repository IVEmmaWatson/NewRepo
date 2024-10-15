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
	UPROPERTY()
	class AZeusCharacter* Character;
	UPROPERTY()
	class AZeusPlayerController* PlayerController;

	// 死亡次数
	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;

public:
	// 重载父类的RPC函数，当score发生变化时会自动调用
	virtual void OnRep_Score() override;
	// 设置score
	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);
	// 网络同步函数重载
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UFUNCTION()
	virtual void OnRep_Defeats();
};

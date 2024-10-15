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

	// ��������
	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;

public:
	// ���ظ����RPC��������score�����仯ʱ���Զ�����
	virtual void OnRep_Score() override;
	// ����score
	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);
	// ����ͬ����������
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UFUNCTION()
	virtual void OnRep_Defeats();
};

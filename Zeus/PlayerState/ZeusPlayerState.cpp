// Fill out your copyright notice in the Description page of Project Settings.


#include "ZeusPlayerState.h"
#include "Zeus/Character/ZeusCharacter.h"
#include"Zeus/PlayerController/ZeusPlayerController.h"

// ���ǿͻ���ִ�еĺ���
void AZeusPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	
	// ���״̬���޷�ֱ�ӻ�ȡ��ҿ������ֻ࣬��getpawn��������ȡ��ҵĽ�ɫʵ����Ȼ��ͨ�����ʵ����ȡ������
	Character = Character == nullptr ? Cast<AZeusCharacter>(GetPawn()) : Character;
	if (Character)
	{
		PlayerController = PlayerController == nullptr ? Cast<AZeusPlayerController>(Character->Controller) : PlayerController;
	}
	if (PlayerController)
	{
		PlayerController->SetHUDScore(Score);
	}
}

// ���Ƿ�������ִ�еĺ���
void AZeusPlayerState::AddToScore(float ScoreAmount)
{
	Score += ScoreAmount;
	Character = Character == nullptr ? Cast<AZeusCharacter>(GetPawn()) : Character;
	if (Character)
	{
		PlayerController = PlayerController == nullptr ? Cast<AZeusPlayerController>(Character->Controller) : PlayerController;
	}
	if (PlayerController)
	{
		PlayerController->SetHUDScore(Score);
	}
}

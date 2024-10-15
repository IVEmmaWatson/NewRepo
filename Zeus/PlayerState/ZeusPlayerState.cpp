// Fill out your copyright notice in the Description page of Project Settings.


#include "ZeusPlayerState.h"
#include "Zeus/Character/ZeusCharacter.h"
#include"Zeus/PlayerController/ZeusPlayerController.h"
#include "Net/UnrealNetwork.h"

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
		PlayerController->SetHUDScore(GetScore());
	}
}

// ���Ƿ�������ִ�еĺ���
void AZeusPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
	Character = Character == nullptr ? Cast<AZeusCharacter>(GetPawn()) : Character;
	if (Character)
	{
		PlayerController = PlayerController == nullptr ? Cast<AZeusPlayerController>(Character->Controller) : PlayerController;
	}
	if (PlayerController)
	{
		PlayerController->SetHUDScore(GetScore());
	}
}

void AZeusPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;
	Character = Character == nullptr ? Cast<AZeusCharacter>(GetPawn()) : Character;
	if (Character)
	{
		PlayerController = PlayerController == nullptr ? Cast<AZeusPlayerController>(Character->Controller) : PlayerController;
	}
	if (PlayerController)
	{
		PlayerController->SetHUDDefeats(Defeats);
	}
}

// UE������ͬ��ϵͳ���Զ����� GetLifetimeReplicatedProps ������ȷ����Щ������Ҫ�ڷ������Ϳͻ���֮����и��ơ�
void AZeusPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// DOREPLIFETIME �꽫��Ҫ���Ƶ�������ӵ� OutLifetimeProps ������
	DOREPLIFETIME(AZeusPlayerState, Defeats);
}

// �ͻ���ִ�еĺ���
void AZeusPlayerState::OnRep_Defeats()
{
	Character = Character == nullptr ? Cast<AZeusCharacter>(GetPawn()) : Character;
	if (Character)
	{
		PlayerController = PlayerController == nullptr ? Cast<AZeusPlayerController>(Character->Controller) : PlayerController;
	}
	if (PlayerController)
	{
		PlayerController->SetHUDDefeats(Score);
	}
}

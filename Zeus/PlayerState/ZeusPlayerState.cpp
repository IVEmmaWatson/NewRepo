// Fill out your copyright notice in the Description page of Project Settings.


#include "ZeusPlayerState.h"
#include "Zeus/Character/ZeusCharacter.h"
#include"Zeus/PlayerController/ZeusPlayerController.h"
#include "Net/UnrealNetwork.h"

// 这是客户端执行的函数
void AZeusPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	
	// 玩家状态类无法直接获取玩家控制器类，只有getpawn方法，获取玩家的角色实例，然后通过玩家实例获取控制器
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

// 这是服务器端执行的函数
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

// UE的网络同步系统会自动调用 GetLifetimeReplicatedProps 函数以确定哪些属性需要在服务器和客户端之间进行复制。
void AZeusPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// DOREPLIFETIME 宏将需要复制的属性添加到 OutLifetimeProps 数组中
	DOREPLIFETIME(AZeusPlayerState, Defeats);
}

// 客户端执行的函数
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

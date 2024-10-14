// Fill out your copyright notice in the Description page of Project Settings.


#include "ZeusPlayerState.h"
#include "Zeus/Character/ZeusCharacter.h"
#include"Zeus/PlayerController/ZeusPlayerController.h"

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
		PlayerController->SetHUDScore(Score);
	}
}

// 这是服务器端执行的函数
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

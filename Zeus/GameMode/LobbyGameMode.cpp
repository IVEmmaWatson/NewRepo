// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// 游戏模式里的游戏状态类
	// 这里是一个智能指针类，需要转换成普通指针类
	// 智能指针的get方法返回智能指针内部管理的原始指针。一个指向智能指针所管理对象的原始指针
	// 用原始指针也就是游戏状态类的一个实例对象去访问玩家数组
	int32 NumberOfPlayer = GameState.Get()->PlayerArray.Num();
	if (NumberOfPlayer == 3)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			bUseSeamlessTravel = true;
			World->ServerTravel(FString("/Game/Maps/ZeusMap1?listen"));
		}
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
}

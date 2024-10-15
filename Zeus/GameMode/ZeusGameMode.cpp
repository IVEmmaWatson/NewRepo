// Fill out your copyright notice in the Description page of Project Settings.


#include "ZeusGameMode.h"
#include "Zeus/Character/ZeusCharacter.h"
#include "Zeus/PlayerController/ZeusPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Zeus/PlayerState/ZeusPlayerState.h"
#include "GameFramework/PlayerStart.h"

// 在 Unreal Engine 中，游戏模式类的代码默认只在服务器端运行。原因是游戏模式负责管理游戏的规则和逻辑，这些应该在服务器端统一处理，以保持游戏的一致性。
// AZeusGameMode 是一个游戏模式类，它的实例在服务器端创建和运行。客户端没有自己的游戏模式实例。
void AZeusGameMode::PlayerEliminated(AZeusCharacter* EliminatedCharacter, AZeusPlayerController* VictimController, AZeusPlayerController* AttackController)
{
	// 攻击者的游戏状态实例
	AZeusPlayerState* AttackerPlayerState = AttackController ? Cast<AZeusPlayerState>(AttackController->PlayerState) : nullptr;
	// 淘汰者的游戏状态实例
	AZeusPlayerState* VictimPlayerState = VictimController ? Cast<AZeusPlayerState>(VictimController->PlayerState) : nullptr;

	// 攻击者涨粉
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);
	}

	// 失败者涨粉
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}

	if (EliminatedCharacter)
	{
		EliminatedCharacter->Elim();
	}
}

void AZeusGameMode::RequestRespawn(ACharacter* ELimmedCharacter, AController* ElimmedController)
{
	if (ELimmedCharacter)
	{
		// >Reset()：这个函数用于重置角色的状态。重置的具体内容取决于 Reset 函数的实现，通常包括恢复角色的初始状态、重置位置、清除状态效果等。
		ELimmedCharacter->Reset();
		ELimmedCharacter->Destroy();
	}

	if (ElimmedController)
	{
		// 模板类数组，因为可以存储任意类型，所以有关键字T,这里存储的是Actor指针
		// APlayerStart 继承自 AActor。在C++中，子类对象可以存储在基类类型的容器中。
		TArray<AActor*> PlayerStarts;
		// GetAllActorsOfClass 在当前游戏模式实例（AZeusGameMode）的上下文中查找所有 APlayerStart 类的实例，并将它们存储在 PlayerStarts 数组中。
		// 调用 UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts) 时，this 只是告诉引擎这个函数是在当前的 AZeusGameMode 上下文中调用的，
		// 但实际查找的是与这个游戏模式关联的世界中的所有玩家起始点。
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		// 在0-所有actor起点直接随机选一个，因为有索引0，所以-1
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num()-1);
		// RestartPlayerAtPlayerStart 接受一个玩家控制器和一个起始点作为参数，然后在该起始点生成一个新的玩家角色，并将其分配给控制器。
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}

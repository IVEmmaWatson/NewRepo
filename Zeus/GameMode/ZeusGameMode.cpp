// Fill out your copyright notice in the Description page of Project Settings.


#include "ZeusGameMode.h"
#include "Zeus/Character/ZeusCharacter.h"
#include "Zeus/PlayerController/ZeusPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Zeus/PlayerState/ZeusPlayerState.h"
#include "GameFramework/PlayerStart.h"

// �� Unreal Engine �У���Ϸģʽ��Ĵ���Ĭ��ֻ�ڷ����������С�ԭ������Ϸģʽ���������Ϸ�Ĺ�����߼�����ЩӦ���ڷ�������ͳһ�����Ա�����Ϸ��һ���ԡ�
// AZeusGameMode ��һ����Ϸģʽ�࣬����ʵ���ڷ������˴��������С��ͻ���û���Լ�����Ϸģʽʵ����
void AZeusGameMode::PlayerEliminated(AZeusCharacter* EliminatedCharacter, AZeusPlayerController* VictimController, AZeusPlayerController* AttackController)
{
	// �����ߵ���Ϸ״̬ʵ��
	AZeusPlayerState* AttackerPlayerState = AttackController ? Cast<AZeusPlayerState>(AttackController->PlayerState) : nullptr;
	// ��̭�ߵ���Ϸ״̬ʵ��
	AZeusPlayerState* VictimPlayerState = VictimController ? Cast<AZeusPlayerState>(VictimController->PlayerState) : nullptr;

	// �������Ƿ�
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);
	}

	// ʧ�����Ƿ�
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
		// >Reset()����������������ý�ɫ��״̬�����õľ�������ȡ���� Reset ������ʵ�֣�ͨ�������ָ���ɫ�ĳ�ʼ״̬������λ�á����״̬Ч���ȡ�
		ELimmedCharacter->Reset();
		ELimmedCharacter->Destroy();
	}

	if (ElimmedController)
	{
		// ģ�������飬��Ϊ���Դ洢�������ͣ������йؼ���T,����洢����Actorָ��
		// APlayerStart �̳��� AActor����C++�У����������Դ洢�ڻ������͵������С�
		TArray<AActor*> PlayerStarts;
		// GetAllActorsOfClass �ڵ�ǰ��Ϸģʽʵ����AZeusGameMode�����������в������� APlayerStart ���ʵ�����������Ǵ洢�� PlayerStarts �����С�
		// ���� UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts) ʱ��this ֻ�Ǹ�����������������ڵ�ǰ�� AZeusGameMode �������е��õģ�
		// ��ʵ�ʲ��ҵ����������Ϸģʽ�����������е����������ʼ�㡣
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		// ��0-����actor���ֱ�����ѡһ������Ϊ������0������-1
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num()-1);
		// RestartPlayerAtPlayerStart ����һ����ҿ�������һ����ʼ����Ϊ������Ȼ���ڸ���ʼ������һ���µ���ҽ�ɫ��������������������
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// ��Ϸģʽ�����Ϸ״̬��
	// ������һ������ָ���࣬��Ҫת������ָͨ����
	// ����ָ���get������������ָ���ڲ������ԭʼָ�롣һ��ָ������ָ������������ԭʼָ��
	// ��ԭʼָ��Ҳ������Ϸ״̬���һ��ʵ������ȥ�����������
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

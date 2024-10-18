// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ZeusPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ZEUS_API AZeusPlayerController : public APlayerController
{
	GENERATED_BODY()
	

public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDAnnouncementCountdown(float CountdownTime);
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

	virtual float GetServerTime();
	virtual void ReceivedPlayer() override;
	void OnMatchStateSet(FName State);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void HandleMatchHasStarted();
protected:

	virtual void BeginPlay() override;
	void SetHUDTime();
	void PollInit();

	/**
	* ͬ���������ͻ���ʱ��
	*/
	
	// �ͻ��������ȡ��������ʱ��,�����ǿͻ��������ʱ��
	UFUNCTION(Server,Reliable)
	void ServerRequestServerTime(float TimeOfClinetRequest);

	// ��������Ӧ���ͻ��˵�ʱ�䣬
	UFUNCTION(Client,Reliable)
	void ClientReportServerTime(float TimeOfClinetRequest, float TimeServerReceivedClientRequest);

	// �ͻ�����������Ĳ���ʱ�䣬�������������10��󣬿ͻ��˲Ž�����Ϸ���������Լ��ı�������ʱ�䣩��Ҫ�����������ͬ������Ҫ�������ʱ��
	float CilentServerDeltaTime = 0.f;

	// ʱ��ͬ��Ƶ��
	UPROPERTY(EditAnywhere, Category = "Time")
	float TimeSyncFrequency = 5.f;

	// ����ͬ��ʱ�䣬ÿ֡����
	float TimeSyncRunningTime = 0.f;
	void CheckTimeSync(float DeltaTime);

	UFUNCTION(Server,Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client,Reliable)
	void ClientJoinMidgame(FName StateOfMatch, float Warmup, float Match, float StartingTime);
private:
	UPROPERTY()
	class AZeusHUD* ZeusHUD;

	float LevelStartingTime=0;
	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	uint32 CountdownInt = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	bool bInitializeCharacterOverlay = false;

	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	int32 HUDDefeats;
};

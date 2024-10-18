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
	* 同步服务器客户端时间
	*/
	
	// 客户端请求获取服务器的时间,参数是客户端请求的时间
	UFUNCTION(Server,Reliable)
	void ServerRequestServerTime(float TimeOfClinetRequest);

	// 服务器响应给客户端的时间，
	UFUNCTION(Client,Reliable)
	void ClientReportServerTime(float TimeOfClinetRequest, float TimeServerReceivedClientRequest);

	// 客户端与服务器的差异时间，比如服务器启动10秒后，客户端才进入游戏（他会有自己的本地启动时间），要与服务器进行同步，就要计算差异时间
	float CilentServerDeltaTime = 0.f;

	// 时间同步频率
	UPROPERTY(EditAnywhere, Category = "Time")
	float TimeSyncFrequency = 5.f;

	// 距离同步时间，每帧更新
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

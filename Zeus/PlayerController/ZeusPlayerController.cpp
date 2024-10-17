// Fill out your copyright notice in the Description page of Project Settings.


#include "ZeusPlayerController.h"
#include "Zeus/HUD/ZeusHUD.h"
#include "Zeus/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Net/UnrealNetwork.h"
#include "Zeus/Character/ZeusCharacter.h"
#include "Zeus/HUD/Announcement.h"
#include "Zeus/GameMode/ZeusGameMode.h"

void AZeusPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ZeusHUD = Cast<AZeusHUD>(GetHUD());
	if(ZeusHUD)
	{
		ZeusHUD->AddAnnouncement();
	}
}

void AZeusPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AZeusPlayerController, MatchState);
}



void AZeusPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	// 这样做的原因是，在 SetHUDHealth 被调用时，不能保证 ZeusHUD 指针一直有效。例如，HUD 可能会在游戏运行过程中被重置或改变
	ZeusHUD = ZeusHUD == nullptr ? Cast<AZeusHUD>(GetHUD()) : ZeusHUD;

	bool bHUDValid = ZeusHUD && 
		ZeusHUD->CharacterOverlay && 
		ZeusHUD->CharacterOverlay->HealthBar && 
		ZeusHUD->CharacterOverlay->HealthText;
	
	if (bHUDValid)
	{
		// 设置进度条
		const float HealthPercent = Health / MaxHealth;
		ZeusHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);

		// CeilToInt 函数将浮点数向上取整为整数
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		ZeusHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void AZeusPlayerController::SetHUDScore(float Score)
{
	ZeusHUD = ZeusHUD == nullptr ? Cast<AZeusHUD>(GetHUD()) : ZeusHUD;
	bool bHUDValid = ZeusHUD &&
		ZeusHUD->CharacterOverlay &&
		ZeusHUD->CharacterOverlay->ScoreAmount;
	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		ZeusHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDScore = Score;
	}
}

// 失败计数
void AZeusPlayerController::SetHUDDefeats(int32 Defeats)
{
	ZeusHUD = ZeusHUD == nullptr ? Cast<AZeusHUD>(GetHUD()) : ZeusHUD;
	bool bHUDValid = ZeusHUD &&
		ZeusHUD->CharacterOverlay &&
		ZeusHUD->CharacterOverlay->DefeatsAmount;
	if (bHUDValid)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		ZeusHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDDefeats = Defeats;
	}
}

// 子弹计数
void AZeusPlayerController::SetHUDAmmo(int32 Ammo)
{
	ZeusHUD = ZeusHUD == nullptr ? Cast<AZeusHUD>(GetHUD()) : ZeusHUD;
	bool bHUDValid = ZeusHUD &&
		ZeusHUD->CharacterOverlay &&
		ZeusHUD->CharacterOverlay->WeaponAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		ZeusHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void AZeusPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	ZeusHUD = ZeusHUD == nullptr ? Cast<AZeusHUD>(GetHUD()) : ZeusHUD;
	bool bHUDValid = ZeusHUD &&
		ZeusHUD->CharacterOverlay &&
		ZeusHUD->CharacterOverlay->CarriedAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		ZeusHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}

}



// OnPossess 函数在玩家控制器控制一个 Pawn 时被调用。
void AZeusPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AZeusCharacter* ZeusCharacter = Cast<AZeusCharacter>(InPawn);
	if (ZeusCharacter)
	{
		SetHUDHealth(ZeusCharacter->GetHealth(), ZeusCharacter->GetMaxtHealth());
	}
}

void AZeusPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();

	CheckTimeSync(DeltaTime);

	PollInit();
}

void AZeusPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && (TimeSyncRunningTime > TimeSyncFrequency))
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}


void AZeusPlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	ZeusHUD = ZeusHUD == nullptr ? Cast<AZeusHUD>(GetHUD()) : ZeusHUD;
	bool bHUDValid = ZeusHUD &&
		ZeusHUD->CharacterOverlay &&
		ZeusHUD->CharacterOverlay->MatchCountdownText;
	if (bHUDValid)
	{
		// CountdownTime总秒数时间，Minutes分钟，
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		// 总秒数-总分中的秒数结果就是剩余的秒数
		int32 Seconds = CountdownTime - Minutes * 60;

		FString CountdownText=FString::Printf(TEXT("%02d:%02d"),Minutes,Seconds);
		ZeusHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}


void AZeusPlayerController::SetHUDTime()
{
	// 计算剩余时间秒数
	uint32 SecondsLeft = FMath::CeilToInt(MatchTime - GetServerTime());
	// 如果当前秒数与剩余秒数不相等，说明一秒过去了，该更新了
	if (CountdownInt != SecondsLeft)
	{
		SetHUDMatchCountdown(MatchTime - GetServerTime());
	}
	// 将剩余秒数存下来
	CountdownInt = SecondsLeft;
}

// 保障初始化
void AZeusPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (ZeusHUD && ZeusHUD->CharacterOverlay)
		{
			CharacterOverlay = ZeusHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				SetHUDHealth(HUDHealth, HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetHUDDefeats(HUDDefeats);
			}
		}
	}
}



void AZeusPlayerController::ServerRequestServerTime_Implementation(float TimeOfClinetRequest)
{
	// 获取当前服务器时间
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClinetRequest, ServerTimeOfReceipt);
}

void AZeusPlayerController::ClientReportServerTime_Implementation(float TimeOfClinetRequest, float TimeServerReceivedClientRequest)
{
	// 数据从客户端到服务器，再从服务器到客户端的来返时间
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClinetRequest;

	// 服务器时间加上半程的来返时间
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5 * RoundTripTime);
	CilentServerDeltaTime = CurrentServerTime - GetWorld()->GetTimeSeconds();
}


float AZeusPlayerController::GetServerTime()
{
	// 服务器返回
	if (HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}
	else
	{
		// 客户端返回
		return GetWorld()->GetTimeSeconds() + CilentServerDeltaTime;
	}
	
}

// 当一个玩家加入游戏或重新生成时，ReceivedPlayer函数会在客户端和服务器上调用，以处理玩家的初始化逻辑
// 这里初始化时间
void AZeusPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void AZeusPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}

void AZeusPlayerController::OnRep_MatchState()
{

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}

void AZeusPlayerController::HandleMatchHasStarted()
{
	ZeusHUD = ZeusHUD == nullptr ? Cast<AZeusHUD>(GetHUD()) : ZeusHUD;
	if (ZeusHUD)
	{
		ZeusHUD->AddCharacterOverlay();
		// 隐藏热身界面的用户控件
		if (ZeusHUD->Announcement)
		{
			ZeusHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

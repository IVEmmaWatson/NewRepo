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
	// ��������ԭ���ǣ��� SetHUDHealth ������ʱ�����ܱ�֤ ZeusHUD ָ��һֱ��Ч�����磬HUD ���ܻ�����Ϸ���й����б����û�ı�
	ZeusHUD = ZeusHUD == nullptr ? Cast<AZeusHUD>(GetHUD()) : ZeusHUD;

	bool bHUDValid = ZeusHUD && 
		ZeusHUD->CharacterOverlay && 
		ZeusHUD->CharacterOverlay->HealthBar && 
		ZeusHUD->CharacterOverlay->HealthText;
	
	if (bHUDValid)
	{
		// ���ý�����
		const float HealthPercent = Health / MaxHealth;
		ZeusHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);

		// CeilToInt ����������������ȡ��Ϊ����
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

// ʧ�ܼ���
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

// �ӵ�����
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



// OnPossess ��������ҿ���������һ�� Pawn ʱ�����á�
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
		// CountdownTime������ʱ�䣬Minutes���ӣ�
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		// ������-�ܷ��е������������ʣ�������
		int32 Seconds = CountdownTime - Minutes * 60;

		FString CountdownText=FString::Printf(TEXT("%02d:%02d"),Minutes,Seconds);
		ZeusHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}


void AZeusPlayerController::SetHUDTime()
{
	// ����ʣ��ʱ������
	uint32 SecondsLeft = FMath::CeilToInt(MatchTime - GetServerTime());
	// �����ǰ������ʣ����������ȣ�˵��һ���ȥ�ˣ��ø�����
	if (CountdownInt != SecondsLeft)
	{
		SetHUDMatchCountdown(MatchTime - GetServerTime());
	}
	// ��ʣ������������
	CountdownInt = SecondsLeft;
}

// ���ϳ�ʼ��
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
	// ��ȡ��ǰ������ʱ��
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClinetRequest, ServerTimeOfReceipt);
}

void AZeusPlayerController::ClientReportServerTime_Implementation(float TimeOfClinetRequest, float TimeServerReceivedClientRequest)
{
	// ���ݴӿͻ��˵����������ٴӷ��������ͻ��˵�����ʱ��
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClinetRequest;

	// ������ʱ����ϰ�̵�����ʱ��
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5 * RoundTripTime);
	CilentServerDeltaTime = CurrentServerTime - GetWorld()->GetTimeSeconds();
}


float AZeusPlayerController::GetServerTime()
{
	// ����������
	if (HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}
	else
	{
		// �ͻ��˷���
		return GetWorld()->GetTimeSeconds() + CilentServerDeltaTime;
	}
	
}

// ��һ����Ҽ�����Ϸ����������ʱ��ReceivedPlayer�������ڿͻ��˺ͷ������ϵ��ã��Դ�����ҵĳ�ʼ���߼�
// �����ʼ��ʱ��
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
		// �������������û��ؼ�
		if (ZeusHUD->Announcement)
		{
			ZeusHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

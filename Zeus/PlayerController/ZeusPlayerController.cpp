// Fill out your copyright notice in the Description page of Project Settings.


#include "ZeusPlayerController.h"
#include "Zeus/HUD/ZeusHUD.h"
#include "Zeus/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Zeus/Character/ZeusCharacter.h"

void AZeusPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ZeusHUD = Cast<AZeusHUD>(GetHUD());
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

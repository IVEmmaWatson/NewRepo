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

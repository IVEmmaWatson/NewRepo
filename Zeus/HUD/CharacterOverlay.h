// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class ZEUS_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
	

public:
	// ����һ����������
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* HealthBar;
	// ����һ���ı�����
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HealthText;


};

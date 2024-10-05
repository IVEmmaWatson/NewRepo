// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ZeusAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ZEUS_API UZeusAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

private:
	// AllowPrivateAccess = true：这个元数据标记允许蓝图访问私有（private）或受保护（protected）的属性。
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = true));
	class AZeusCharacter *ZeusCharacter;

	// 速度
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess=true));
	float Speed;

	// 是否在空中
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true));
	bool bIssInAir;

	// 是否处于加速
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true));
	bool bIsAccelerating;

	// 是否装备了武器
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true));
	bool bWeaponEquipped;

	// 是否处于下蹲
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true));
	bool bIsCrouched;

	// 是否处于瞄准状态
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true));
	bool bAiming;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true));
	float ShuiPing;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true));
	float ChuiZhi;

	FRotator CharacterRotatorLastFrame;
	FRotator CharacterRotator;
	FRotator DeltaRotation;
};

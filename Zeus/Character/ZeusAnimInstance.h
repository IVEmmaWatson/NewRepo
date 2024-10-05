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
	// AllowPrivateAccess = true�����Ԫ���ݱ��������ͼ����˽�У�private�����ܱ�����protected�������ԡ�
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = true));
	class AZeusCharacter *ZeusCharacter;

	// �ٶ�
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess=true));
	float Speed;

	// �Ƿ��ڿ���
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true));
	bool bIssInAir;

	// �Ƿ��ڼ���
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true));
	bool bIsAccelerating;

	// �Ƿ�װ��������
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true));
	bool bWeaponEquipped;

	// �Ƿ����¶�
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true));
	bool bIsCrouched;

	// �Ƿ�����׼״̬
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

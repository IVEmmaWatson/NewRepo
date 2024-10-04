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

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess=true));
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true));
	bool bIssInAir;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true));
	bool bIsAccelerating;
};

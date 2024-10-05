// Fill out your copyright notice in the Description page of Project Settings.


#include "ZeusAnimInstance.h"
#include "ZeusCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UZeusAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	// TryGetPawnOwner �� Unreal Engine �� UAnimInstance ���һ����Ա���������ڻ�ȡӵ�е�ǰ����ʵ���� APawn��
	// �������ͨ���ڶ�����ͼ��ʹ�ã��Ա��ȡ���Ƹö�����ͼ�Ľ�ɫ��
	ZeusCharacter = Cast<AZeusCharacter>(TryGetPawnOwner());
}

void UZeusAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	if (ZeusCharacter == nullptr)
	{
		ZeusCharacter = Cast<AZeusCharacter>(TryGetPawnOwner());
	}

	if (ZeusCharacter == nullptr) return;

	// ��ȡ��ɫ���ٶ�ʸ��
	// GetVelocity() �� Unreal Engine �� ACharacter ���һ����Ա���������ڻ�ȡ��ɫ��ǰ���ٶ�ʸ����FVector����
	// �����������һ�� FVector����ʾ��ɫ����������ϵ�е��ٶȣ����� X��Y �� Z �����ϵķ��������а�����ɫ�� X��Y �� Z �����ϵ��ٶȷ�����
	// �����ɫ���ٶ�ʸ��Ϊ FVector(100, 0, -50)��X �����ٶȷ���Ϊ 100����ʾ��ɫ�����ƶ���
	// Y �����ٶȷ���Ϊ 0����ʾ��ɫ��ǰ������û���ƶ���
	// Z �����ٶȷ���Ϊ - 50����ʾ��ɫ���½���
	// Size()���� FVector ���һ����Ա���������ڼ���ʸ����ŷ����ó��ȣ���ʸ���Ĵ�С����
	// 

	FVector Velocity = ZeusCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	// ����ɫ�Ƿ��ڿ���
	//  ZeusCharacter->GetCharacterMovement()�����������һ��ָ�� UCharacterMovementComponent ��ָ�룬
	//  ������������ɫ�������ƶ��߼����������ߡ���Ծ�͵��䡣
	//  IsFalling()���� UCharacterMovementComponent ���һ����Ա������������һ������ֵ��true �� false������ʾ��ɫ��ǰ�Ƿ��ڵ��䣨�����Ӵ����棩��
	bIssInAir = ZeusCharacter->GetCharacterMovement()->IsFalling();

	// GetCurrentAcceleration��ȡ��ɫ���ٶ�ʸ����Size��ȡ��ɫ���ٶ�����
	bIsAccelerating = ZeusCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0 ? true : false;
	bWeaponEquipped = ZeusCharacter->IsWeaponEquipped();
	bIsCrouched = ZeusCharacter->bIsCrouched;
}

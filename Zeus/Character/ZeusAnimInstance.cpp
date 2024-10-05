// Fill out your copyright notice in the Description page of Project Settings.


#include "ZeusAnimInstance.h"
#include "ZeusCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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

	// ��ȡ��ɫ��xyzȫ������ٶ�ʸ��
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
	bAiming = ZeusCharacter->IsAiming();

	// ��ȡ��ɫ��������׼����ֵ���ǿ�������ֵ�����ǽ�ɫ�ķ���ֵ�����Ǹ���������
	FRotator AimRotation=ZeusCharacter->GetBaseAimRotation();
	// UKismetMathLibrary::MakeRotFromX����һ����̬���������ڸ��ݸ����� X ����������һ����תֵ��FRotator ���ͣ���
	// ������˵�����ᴴ��һ����ת��ʹ�� X ����뵽��������������
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ZeusCharacter->GetVelocity());

	// ZeusCharacter->HasAuthority() && !ZeusCharacter->IsLocallyControlled(),��������������ҵľ���Ȩ�������������ֲ��ǿͻ��˽�ɫ�ı��ؿ�����
	// if (!ZeusCharacter->HasAuthority() && !ZeusCharacter->IsLocallyControlled())
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("movementrotation ShuiPing %f:"), MovementRotation.Yaw);
	//  	UE_LOG(LogTemp, Warning, TEXT("AimRotation ShuiPing %f:"), AimRotation.Yaw);
	// }

	// ���������׼������ƶ�����֮���ˮƽ�ǶȲ�ֵ
	// ʹ�ò�ֵ���� RInterpTo() �� DeltaRotation ��֡ƽ���ع��ɵ�Ŀ����ת DeltaRot������ DeltaTime ����ʱ���ϵ�ƽ�����ɣ�
	// 15.f ���ǲ�ֵ�ٶȡ�������������ת������������Ȼ����ֹͻأ����ת���䡣
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 15.f);
	ShuiPing = DeltaRotation.Yaw;
	// ShuiPing = AimRotation.Yaw;
	// ShuiPing = MovementRotation.Yaw;

	// ������һ֡��ɫ����ת�����ں���������ת�仯��
	CharacterRotatorLastFrame = CharacterRotator;
	// ��ȡ��ǰ֡��ɫ��ʵ����תֵ CharacterRotator�������ת��ʾ��ɫ�ڳ����е�ʵ�ʷ���
	CharacterRotator = ZeusCharacter->GetActorRotation();
	// �����ɫ��ǰ��ת����һ֡��ת֮��Ĳ��죬Delta ��ʾ��֮֡�����ת�仯������ȷ����ɫ����һ֡����ת�˶��١�
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotator, CharacterRotatorLastFrame);
	// �� Delta �е� Yaw ֵ���� DeltaTime���Լ�����ת�ٶȡ�Target �����ɫÿ���ӵ�ˮƽ��ת���ʡ�
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(ChuiZhi, Target, DeltaTime, 6.f);
	ChuiZhi=FMath::Clamp(Interp, -90.f, 90.f);
	
	// �����������ɫ���˶�����תʱ�Ĳ�ֵ��ת��ƽ�����ɡ�50-83�д�����Ҫ��дע��
}


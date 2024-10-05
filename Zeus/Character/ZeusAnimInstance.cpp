// Fill out your copyright notice in the Description page of Project Settings.


#include "ZeusAnimInstance.h"
#include "ZeusCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UZeusAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	// TryGetPawnOwner 是 Unreal Engine 中 UAnimInstance 类的一个成员函数，用于获取拥有当前动画实例的 APawn。
	// 这个函数通常在动画蓝图中使用，以便获取控制该动画蓝图的角色。
	ZeusCharacter = Cast<AZeusCharacter>(TryGetPawnOwner());
}

void UZeusAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	if (ZeusCharacter == nullptr)
	{
		ZeusCharacter = Cast<AZeusCharacter>(TryGetPawnOwner());
	}

	if (ZeusCharacter == nullptr) return;

	// 获取角色的速度矢量
	// GetVelocity() 是 Unreal Engine 中 ACharacter 类的一个成员函数，用于获取角色当前的速度矢量（FVector）。
	// 这个函数返回一个 FVector，表示角色在世界坐标系中的速度，包括 X、Y 和 Z 方向上的分量。其中包含角色在 X、Y 和 Z 方向上的速度分量。
	// 假设角色的速度矢量为 FVector(100, 0, -50)：X 方向速度分量为 100，表示角色向右移动。
	// Y 方向速度分量为 0，表示角色在前后方向上没有移动。
	// Z 方向速度分量为 - 50，表示角色在下降。
	// Size()这是 FVector 类的一个成员函数，用于计算矢量的欧几里得长度（即矢量的大小）。
	// 

	FVector Velocity = ZeusCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	// 检查角色是否在空中
	//  ZeusCharacter->GetCharacterMovement()这个函数返回一个指向 UCharacterMovementComponent 的指针，
	//  该组件负责处理角色的所有移动逻辑，包括行走、跳跃和掉落。
	//  IsFalling()这是 UCharacterMovementComponent 类的一个成员函数，它返回一个布尔值（true 或 false），表示角色当前是否在掉落（即不接触地面）。
	bIssInAir = ZeusCharacter->GetCharacterMovement()->IsFalling();

	// GetCurrentAcceleration获取角色加速度矢量，Size获取角色加速度总量
	bIsAccelerating = ZeusCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0 ? true : false;
	bWeaponEquipped = ZeusCharacter->IsWeaponEquipped();
	bIsCrouched = ZeusCharacter->bIsCrouched;
}

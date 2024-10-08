// Fill out your copyright notice in the Description page of Project Settings.


#include "ZeusAnimInstance.h"
#include "ZeusCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Zeus/Weapon/Weapon.h"

void UZeusAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	//？ TryGetPawnOwner 是 Unreal Engine 中 UAnimInstance 类的一个成员函数，用于获取拥有当前动画实例的 APawn。
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

	// 获取角色的xyz全方向的速度矢量
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

	// UE_LOG(LogTemp, Display, TEXT("Speed: %f"), Speed);

	// FVector fangxiang = Velocity.GetSafeNormal();
	// UE_LOG(LogTemp, Warning, TEXT("fangxiang: X=%f, Y=%f, Z=%f"), fangxiang.X, fangxiang.Y, fangxiang.Z);
	// UE_LOG(LogTemp, Display, TEXT("Velocity: X=%f, Y=%f, Z=%f"), Velocity.X, Velocity.Y, Velocity.Z);
	// 检查角色是否在空中
	//  ZeusCharacter->GetCharacterMovement()这个函数返回一个指向 UCharacterMovementComponent 的指针，
	//  该组件负责处理角色的所有移动逻辑，包括行走、跳跃和掉落。
	//  IsFalling()这是 UCharacterMovementComponent 类的一个成员函数，它返回一个布尔值（true 或 false），表示角色当前是否在掉落（即不接触地面）。
	bIssInAir = ZeusCharacter->GetCharacterMovement()->IsFalling();

	// GetCurrentAcceleration获取角色加速度矢量，Size获取角色加速度总量
	bIsAccelerating = ZeusCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0 ? true : false;
	bWeaponEquipped = ZeusCharacter->IsWeaponEquipped();
	EquippedWeapon = ZeusCharacter->GetEquippedWeapon();
	bIsCrouched = ZeusCharacter->bIsCrouched;
	bAiming = ZeusCharacter->IsAiming();

	
	// 获取角色基础的瞄准方向值（是控制器的值，不是角色的方向值），是个方向向量
	FRotator AimRotation=ZeusCharacter->GetBaseAimRotation();
	// UKismetMathLibrary::MakeRotFromX这是一个静态函数，用于根据给定的 X 轴向量生成一个旋转值（FRotator 类型）。
	// 具体来说，它会创建一个旋转，使得 X 轴对齐到给定的向量方向。
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ZeusCharacter->GetVelocity());
	// UE_LOG(LogTemp, Warning, TEXT("AimRotation ShuiPing %f:"), AimRotation.Yaw);这个就是鼠标转向的值确定了
	// ZeusCharacter->HasAuthority() && !ZeusCharacter->IsLocallyControlled(),服务器有所有玩家的绝对权威，而服务器又不是客户端角色的本地控制器
	// if (!ZeusCharacter->HasAuthority() && !ZeusCharacter->IsLocallyControlled())
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("movementrotation ShuiPing %f:"), MovementRotation.Yaw);
	//  	UE_LOG(LogTemp, Warning, TEXT("AimRotation ShuiPing %f:"), AimRotation.Yaw);
	// }

	// 计算计算瞄准方向和移动方向之间的水平角度差值
	// 使用插值函数 RInterpTo() 将 DeltaRotation 逐帧平滑地过渡到目标旋转 DeltaRot，参数 DeltaTime 控制时间上的平滑过渡，
	// 15.f 则是插值速度。这样可以让旋转看起来更加自然，防止突兀的旋转跳变。
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 15.f);
	ShuiPing = DeltaRotation.Yaw;
	// ShuiPing = AimRotation.Yaw;
	// ShuiPing = MovementRotation.Yaw;

	// 保存上一帧角色的旋转，用于后续计算旋转变化。
	CharacterRotatorLastFrame = CharacterRotator;
	// 获取当前帧角色的实际旋转值 CharacterRotator，这个旋转表示角色在场景中的实际方向。
	CharacterRotator = ZeusCharacter->GetActorRotation();
	// 计算角色当前旋转和上一帧旋转之间的差异，Delta 表示两帧之间的旋转变化，用于确定角色在这一帧内旋转了多少。
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotator, CharacterRotatorLastFrame);
	// 将 Delta 中的 Yaw 值除以 DeltaTime，以计算旋转速度。Target 代表角色每秒钟的水平旋转速率。
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(ChuiZhi, Target, DeltaTime, 6.f);
	ChuiZhi=FMath::Clamp(Interp, -90.f, 90.f);
	
	// UE_LOG(LogTemp, Warning, TEXT("shuiping is %f...chuizhi is %f"),ShuiPing,ChuiZhi);
	// 上面代码计算角色在运动和旋转时的插值旋转和平滑过渡。50-83行代码需要重写注释

	AO_Yaw = ZeusCharacter->GETAO_Yaw();
	AO_Pitch = ZeusCharacter->GETAO_Pitch();

	//? 检查是否装备了武器和武器是否存在,以及武器模型人物模型是否存在
	if (bWeaponEquipped && EquippedWeapon&& EquippedWeapon->GetWeaponMesh()&&ZeusCharacter->GetMesh())
	{
		// LeftHandTransform类型是一个类的旋转、位置、缩放三个属性的变量，这里是获取通过武器实例调用获取武器的骨骼然后获取骨骼上名为LeftHandSocket的插槽，
		// ERelativeTransformSpace::RTS_World: 这是一个枚举值，指定要获取的变换空间。在这里，RTS_World 表示获取插槽在世界空间中的变换。
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		// 武器插槽在世界空间的位置转换映射到角色右手骨骼的空间位置上，然后获取其位置和旋转向量
		ZeusCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

	}
}


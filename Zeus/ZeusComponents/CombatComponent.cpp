// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Zeus/Weapon/Weapon.h"
#include "Zeus/Character/ZeusCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"


#define TRACE_LENGTH 80000.f
// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	// 不需要每帧都更新，因为这是把武器拿在手上的动作
	PrimaryComponentTick.bCanEverTick = true;


	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;
	// ...
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquipedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
}

// 这个函数只会在服务器上执行
void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	// 检查角色指针和武器指针是否为空
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	 
	EquipedWeapon = WeaponToEquip;

	EquipedWeapon->SetWeaponState(EWeaponState::EWS_Equiped);
	// 从角色骨骼获取手部插槽
	// AttachActor：这个方法将一个 Actor 附加到插槽上。
	// 第一个参数是要附加的 Actor（在你的例子中是 EquipedWeapon），第二个参数是要附加到的骨骼网格。
	const USkeletalMeshSocket* HandSocket=Character->GetMesh()->GetSocketByName(FName("righthandsocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(EquipedWeapon,Character->GetMesh());
	}
	// 设置这个武器实例的拥有者
	EquipedWeapon->SetOwner(Character);
	
	// 当这个属性为 false 时，角色不会根据移动方向来调整自身的朝向。
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	// 设置角色跟着控制器旋转，当这个属性为 true时，角色会根据控制器的旋转来调整自身的朝向。
	Character->bUseControllerRotationYaw = true;
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...设置角色移动速度
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed= BaseWalkSpeed;
	}

	
	
}

// 通过函数设置是否在瞄准，判断是否是服务器角色，如果是客户端角色则调用rpc函数
// 因为客户端是复制角色没有与服务器的实际角色进行通信只能在本地看到自己的姿势
// 所以当客户端按下瞄准后需要通知服务器角色也进入瞄准
void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming; // 这句可以使客户端按下就可以看到瞄准姿势，不然还要等下面的rpc函数调用后才能看到姿势
	if (!Character->HasAuthority())
	{
		ServerSetAiming(bIsAiming);
	}
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;


	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

// 当EquipedWeapon变量发生变化时调用此函数改变玩家的旋转朝向
// 需要当玩家拿着武器时，角色移动方向随鼠标控制，就是拿着武器的一面始终朝前，即相当于第一人称模式
void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquipedWeapon && Character)
	{
		// 当这个属性为 false 时，角色不会根据移动方向来调整自身的朝向。
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		// 设置角色跟着控制器旋转，当这个属性为 true时，角色会根据控制器的旋转来调整自身的朝向。
		Character->bUseControllerRotationYaw = true;
	}
}

// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// HitResult用来存储射线命中的目标信息
	FHitResult HitResult;
	TraceUnderCorsshairs(HitResult);
	
	
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed; 
	if (bFireButtonPressed)
	{
		ServerFire();
	}
}

//t FHitResult是一个包含了射线检测命中的详细信息。成员变量ImpactPoint：命中的位置。Normal：命中点的法线。Actor：命中的 Actor。Component：命中的组件。
// TraceHitResult参数要手动定义和传参
// _TraceUnderCorsshairs函数通常用于在游戏中进行射线检测
void UCombatComponent::TraceUnderCorsshairs(FHitResult& TraceHitResult)
{
	// 获取游戏屏幕当前的尺寸
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// 获取游戏屏幕中间的坐标
	FVector2D CrosshariLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	FVector CrosshariWorldPosition;	// 假设 WorldPosition 的值是 (1000, 500, 200)。这表示屏幕上的点 (100, 200) 在3D世界中的具体位置是 (1000, 500, 200)。
	FVector CrosshariWorldDirection;	// 假设 WorldDirection 的值是 (0.707, 0.707, 0)。这表示从相机位置到 WorldPosition 的方向向量。这个向量是单位向量，表示方向而不是具体的位置。
	// 将一个二维坐标转换成三维坐标，参数1玩家控制器指针，参数2要转换的二维坐标，参数3转换后的世界空间3D坐标，参数4转换后的世界空间方向
	bool bScreenToWorld=UGameplayStatics::DeprojectScreenToWorld(
		PlayerController,CrosshariLocation, CrosshariWorldPosition, CrosshariWorldDirection
	);

	
	// UGameplayStatics::GetPlayerController(this, 0);
	if (bScreenToWorld)
	{
		// 初始位置，即屏幕中间点的三维位置坐标，一般时摄像机的坐标
		// 屏幕中心点的位置（CrosshariWorldPosition）通常接近于摄像机的位置，因为它是从摄像机视角转换过来的。
		// 由于 DeprojectScreenToWorld 方法将屏幕中心点投影到世界坐标，结果会接近摄像机的实际位置。
		FVector Start = CrosshariWorldPosition;

		// 将方向向量缩放变成缩放向量
		FVector End = Start + CrosshariWorldDirection * 80000.f;

		
		// 射线从start开始，沿着 WorldDirection 方向延伸80000单位，检测是否碰撞到任何物体。并将碰撞后的目标信息存在TraceHitResult里
		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);

		// 检测碰撞,如果没有发生碰撞，那就把终点位置设为碰撞点（一般是天空）
		if (!TraceHitResult.bBlockingHit)
		{
			// 碰撞点的坐标
			TraceHitResult.ImpactPoint = End;
			HitTarget = End;
		}
		else
		{
			HitTarget = TraceHitResult.ImpactPoint;
			// 绘制球体半径
			// 如果 bBlockingHit 为 true（即发生了碰撞），则调用 DrawDebugSphere 函数在碰撞点绘制一个红色的调试球体。
			DrawDebugSphere(
				GetWorld(),
				TraceHitResult.ImpactPoint,
				12.f,	// 球体的半径
				12,	// 球体的细分数（即球体的分段数，用于控制球体的平滑度）。
				FColor::Red	// 球体的颜色，红色。
			);
		}
	}



}

// 多播RPC调用，在服务器端调用多播rpc
void UCombatComponent::MulticastFire_Implementation()
{
	if (EquipedWeapon == nullptr) return;
	if (Character )
	{
		Character->PlayFireMontage(bAiming);
		
		EquipedWeapon->Fire(HitTarget);
	}
}

// 远程RPC调用
void UCombatComponent::ServerFire_Implementation()
{
	MulticastFire();
}
	


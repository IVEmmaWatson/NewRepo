// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Zeus/Weapon/Weapon.h"
#include "Zeus/Character/ZeusCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Zeus/PlayerController/ZeusPlayerController.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"


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
	if (EquipedWeapon)
	{
		EquipedWeapon->Dropped();
	 }
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
	EquipedWeapon->SetHUDAmmo();
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

		if (Character->GetFollowCamera())
		{
			// 获取相机的默认视野值
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
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
		// 客户端也执行一遍这个设置武器状态和附加插槽，防止高延迟的情况下客户端需要等待同步，期间武器可能在错误的位置。
		EquipedWeapon->SetWeaponState(EWeaponState::EWS_Equiped);
		// 从角色骨骼获取手部插槽
		// AttachActor：这个方法将一个 Actor 附加到插槽上。
		// 第一个参数是要附加的 Actor（在你的例子中是 EquipedWeapon），第二个参数是要附加到的骨骼网格。
		const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("righthandsocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(EquipedWeapon, Character->GetMesh());
		}

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

	
	
	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCorsshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;

		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}
	
}

// 只当开火箭按下后才追踪射线
void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed; 
	if (bFireButtonPressed&&EquipedWeapon)
	{
		// HitResult用来存储射线命中的目标信息
		
		Fire();
	}
}

void UCombatComponent::Fire()
{

	if (CanFire())
	{
		bCanFire = false;
		ServerFire(HitTarget);

		if (EquipedWeapon)
		{
			CrosshairShootingFactor = 0.52f;
		}
		StartFireTimer();
	}
	
}

// 假设 FireDelay 设置为1秒，那么当玩家按下开火按钮时，定时器会开始计时，
// 1秒后调用 FireTimerFinished 函数。FireTimerFinished 函数会检查是否需要再次开火，或者停止定时器以防止连续开火。


void UCombatComponent::StartFireTimer()
{
	if (EquipedWeapon == nullptr || Character == nullptr) return;
	Character->GetWorldTimerManager().SetTimer(
		// 这是一个 FTimerHandle 类型的变量，用于存储和管理这个定时器的句柄。
		FireTimer,
		// 这是一个指向当前对象的指针，即 UCombatComponent 实例。
		this,
		// 这是一个函数指针，指向 UCombatComponent 类中的 FireTimerFinished 函数。
		&UCombatComponent::FireTimerFinished,
		// 这是定时器的延迟时间,定时器将等待指定的时间，然后触发 FireTimerFinished 函数。
		EquipedWeapon->FireDelay
	);
}

void UCombatComponent::FireTimerFinished()
{
	if (EquipedWeapon == nullptr) return;
	bCanFire = true;
	if (bFireButtonPressed && EquipedWeapon->bAutomatic)
	{
		Fire();
	}
}

bool UCombatComponent::CanFire()
{
	if (EquipedWeapon == nullptr) return false;
	// 如果子弹不为空，和bCanFire为能开枪
	return !EquipedWeapon->IsEmpty() && bCanFire;
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

		// 计算初始线到角色之间的距离，因为初始线是从相机发射出来的，而瞄准线不能指向角色后面，也就是相机到角色的这一段距离
		if (Character)
		{
			// Size() 函数计算向量的长度，即向量的欧几里得距离。
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();

			// 更新射线起点。
			// 在三维空间中，方向向量（CrosshairWorldDirection）提供了一个明确的方向，而标量（DistanceToCharacter + 100.f）只是表示一个距离。
			// 要在特定方向上移动一定距离，就必须将方向向量与距离相乘。这确保我们沿着正确的方向移动。
			Start += CrosshariWorldDirection * (DistanceToCharacter + 100.f);

			
		}



		// 将方向向量缩放变成缩放向量
		FVector End = Start + CrosshariWorldDirection * 80000.f;

		

		// 射线从start开始，沿着 WorldDirection 方向延伸80000单位，检测是否碰撞到任何物体。并将碰撞后的目标信息存在TraceHitResult里
		// 如果没有碰撞到物体，那TraceHitResult的命中点默认为000，就会飞到世界原点
		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);

		// 当没有碰撞到物体时，防止子弹回世界原点
		if (!TraceHitResult.bBlockingHit)
		{
			// 碰撞点的坐标
			TraceHitResult.ImpactPoint = End;
			// HitTarget = End;
		}

		// TraceHitResult.GetActor()返回碰撞到的actor对象指针，Implements<UInteractWithCrosshairInterface>() 是一个模板函数，用于检查给定的 AActor 是否实现了某个接口。
		// 这里是检查碰撞到的这个actor类是否实现了这个接口
		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairInterface>())
		{
			HUDPackage.CrosshairColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairColor = FLinearColor::White;
		}
		// 不需要检测射线碰撞，上面已经检测碰撞了，这只是方面测试射线有没有生效
		/*
		// 检测射线碰撞,如果没有发生碰撞，那就把终点位置设为碰撞点（一般是天空）
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
		*/
	}



}


// 多播RPC调用，在服务器端调用多播rpc
//t FVector_NetQuantize 是 FVector 的一个优化版本，通过量化的方式降低了数据的精度和大小，
//t 从而提高了网络传输效率，适用于多人游戏中频繁同步的位置和方向数据。
void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize TraceHitTarget)
{
	if (EquipedWeapon == nullptr) return;
	if (Character )
	{
		Character->PlayFireMontage(bAiming);
		
		EquipedWeapon->Fire(TraceHitTarget);
	}
}

// 远程RPC调用
void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}
	

// 设置HUD纹理,初始化纹理
void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (Character == nullptr || Character->Controller==nullptr) return;

	// Controller这个是自定义控制器的变量，如果这个变量没有值就把角色的默认控制器的值给他
	Controller=Controller == nullptr ? Cast<AZeusPlayerController>(Character->GetController()) : Controller;


	if (Controller)
	{
		// APlayerController是专门用于处理玩家输入、相机控制和HUD管理的类
		// HUD 是显示在屏幕上的图形界面元素，比如玩家的健康条、分数、弹药等信息。
		HUD = HUD == nullptr ? Cast<AZeusHUD>(Controller->GetHUD()) : HUD;
		if (HUD)
		{
			
			if (EquipedWeapon)
			{
				
				HUDPackage.CrosshairsCenter = EquipedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquipedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquipedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsTop = EquipedWeapon->CrosshairsTop;
				HUDPackage.CrosshairsBottom = EquipedWeapon->CrosshairsBottom;

				
			}
			else
			{
				
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;

				
			}
			// 在调用绘制准星hud之前，设置准星分散值,根据角色的速度值
			// 定义一个二维向量（0.f：行走速度的最小值。MaxWalkSpeed行走速度的最大值）
			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			// 映射到准星分散的范围
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			// 获取角色的水平方向速度
			FVector Vectory = Character->GetVelocity();
			Vectory.Z = 0;

			// 将角色当前速度从WalkSpeedRange映射到VelocityMultiplierRange
			// 假设角色的最大行走速度是600，如果当前速度是300。Vectory.Size() 返回300。
			// FMath::GetMappedRangeValueClamped 将300从范围 [0, 600] 映射到 [0, 1]，结果是 0.5。CrosshairVelocityFactor 因此为 0.5
			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Vectory.Size());

			if (Character->GetCharacterMovement()->IsFalling())
			{
				//t 插值函数用于平滑地过渡某个值到目标值。
				//t 参数1当前值（CrosshairInAirFactor）：当前的准星因子值。参数2目标值（例如，2.25f 或 0.f）：期望插值到的目标值。
				//t DeltaTime：时间间隔，用于确保插值过程的平滑性。
				//t 插值速度（2.25f 或 30.f）：插值的速度，数值越大，过渡越快。
				//t 就是根据插值速度和帧率，把CrosshairInAirFactor值过渡到2.25。插值速度越快，过渡也就越快
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
			}

			if (bAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, -0.58f, DeltaTime, 30.f);
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
			}

			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 40.f);

			HUDPackage.CrosshairSpread = 
				0.5f+
				CrosshairVelocityFactor+
				CrosshairInAirFactor+
				CrosshairAimFactor+
				CrosshairShootingFactor;

			HUD->SetHUDPackeage(HUDPackage);
		}
	}
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquipedWeapon == nullptr) return;
	if (bAiming)
	{
		// 获取当前武器的视野值和插值速度，让当前视野值平滑的过渡到当前武器的视野值
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquipedWeapon->GetZoomedFOV(), DeltaTime, EquipedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		// 当不在瞄准的时候，平滑过渡到默认的视野值
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}

	if (Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}


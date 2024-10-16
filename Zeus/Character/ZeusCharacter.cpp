// Fill out your copyright notice in the Description page of Project Settings.


#include "ZeusCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Zeus/Weapon/Weapon.h"
#include "Zeus/ZeusComponents/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Zeus/Character/ZeusAnimInstance.h"
#include "Zeus/PlayerController/ZeusPlayerController.h"
#include "Zeus/Zeus.h"
#include "Kismet/GameplayStatics.h"
#include "Zeus/GameMode/ZeusGameMode.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Zeus/PlayerState/ZeusPlayerState.h"
#include "TimerManager.h"
#include "Zeus/Weapon/WeaponTypes.h"

AZeusCharacter::AZeusCharacter()
{
	// 这行代码用于启用或禁用 Actor 的 Tick 函数
	PrimaryActorTick.bCanEverTick = true;

	// 创建并初始化一个 USpringArmComponent（弹簧臂组件）
	// CameraBoom 是一个指向新创建的 USpringArmComponent 的指针。
	// CreateDefaultSubobject 是一个模板函数，用于创建一个新的子对象（在这里是 USpringArmComponent 类型）
	//  TEXT("CameraBoom") 是这个组件的名称，用于标识和调试。
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	// 将弹簧臂组件附加到角色的骨骼组件上，这样，CameraBoom 和它附加的摄像机将会跟随角色的网格移动和旋转。
	// GetMesh() 是一个函数，通常用于返回角色的网格组件（USkeletalMeshComponent）。这个网格组件是角色的可视化表示，包含了角色的骨骼和动画数据。
	CameraBoom->SetupAttachment(GetMesh());
	// 弹簧臂的长度
	CameraBoom->TargetArmLength = 600.f;
	// 使弹簧臂使用角色的控制旋转。这意味着摄像机将跟随角色的旋转方向，通常用于第三人称视角，使摄像机始终对准角色的前方。
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// 绑定到弹簧臂上，默认插槽上，弹簧臂组件（USpringArmComponent）通常直接附加到角色的根组件或网格组件上，而不需要指定特定的插槽。
	// 弹簧臂组件本身没有多个插槽，它的默认行为是将附加的组件（如摄像机）放置在弹簧臂的末端。
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// 角色不与控制器一起旋转
	// 当这个属性为 false 时，角色不会根据控制器的旋转来调整自身的朝向。也就是说，角色的朝向不再直接跟随控制器的旋转。
	bUseControllerRotationYaw = false; 
	// 当这个属性为 true 时，角色会根据移动方向来调整自身的朝向。也就是说，角色会自动面向它移动的方向。
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// 创建显示文本控件实例，绑定控件到根组件
	OverHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));
	OverHeadWidget->SetupAttachment(RootComponent);

	// 创建战斗附加组件实例，不绑定到根组件，因为要绑定到角色手部插槽里
	// 战斗类将为我们的角色处理所有与战斗相关的功能，这是这个类的实例，通过这个实例可以调用战斗类的所有变量和方法
	// component组件不需要注册为网络复制属性，调用方法设置即可
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	// 这行代码设置 CombatComponent 为可复制的，这意味着它的状态将在服务器和客户端之间同步。
	Combat->SetIsReplicated(true);

	// 通过将 bCanCrouch 设置为 true，你允许角色执行蹲下动作。
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	// 设置骨骼体的碰撞通道类型
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()-> SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	// 角色的转身速度
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 100.f);


	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));
}


void AZeusCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	UpdateHUDHealth();
	if (HasAuthority())
	{
		// OnTakeAnyDamage是多播委托类型，用于广播接收到的伤害事件
		OnTakeAnyDamage.AddDynamic(this, &AZeusCharacter::ReceiveDamage);
	}
}

// 只是触发一次初始同步，不会改变实际的分数值。
void AZeusCharacter::PollInit()
{
	if (ZeusPlayerState == nullptr)
	{
		ZeusPlayerState = GetPlayerState<AZeusPlayerState>();
		if (ZeusPlayerState)
		{
			ZeusPlayerState->AddToScore(0.f);
			ZeusPlayerState->AddToDefeats(0);
		}
	}
}

void AZeusCharacter::UpdateHUDHealth()
{
	ZeusPlayerController = ZeusPlayerController == nullptr ? Cast<AZeusPlayerController>(Controller) : ZeusPlayerController;
	if (ZeusPlayerController)
	{
		ZeusPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

// 只有按W才会触发这个函数，所以你单纯晃鼠标不会获取当前的yaw值，只有按了W才会获取此时鼠标的yaw值
void AZeusCharacter::MoveForward(float Value)
{
	// Controller是从Apwan类继承来的，表示当前控制这个角色的控制器
	if (Controller != nullptr && Value != 0.f)
	{
		// Controller->GetControlRotation().Yaw获取玩家控制器的当前控制旋转的 Yaw 值。
		// 以当前yaw值、忽略其他的值创建一个旋转类对象，因为yaw代表玩家的水平旋转也就是角色朝向，把其他值固定住

		// 这个是创建一个以获取鼠标控制器的yaw值为核心，俯仰角和roll角固定为0 的旋转向量
		// 然后下面以这个旋转向量创建一个旋转矩阵，获取这个矩阵的X轴的值
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		// FRotationMatrix(YawRotation)：使用 YawRotation 创建一个旋转矩阵。
		// GetUnitAxis(EAxis::X)：从旋转矩阵中获取 X 轴的单位向量。
		// FVector Direction：将结果存储在一个 FVector 对象中，表示方向向量。因为X轴始终表示角色正前方
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		// UE_LOG(LogTemp, Warning, TEXT("Direction: X=%f, Y=%f, Z=%f"), Direction.X, Direction.Y, Direction.Z);

		// UE_LOG(LogTemp, Warning, TEXT("YawRotation: X=%f, Y=%f, Z=%f"), YawRotation.Roll, YawRotation.Yaw, YawRotation.Pitch);
		// AddMovementInput 是 ACharacter 类中的一个方法，用于向角色添加移动输入。
		// 这句代码的作用是根据指定的方向和输入值来移动角色。
		// value正值表示向前，负值表示向后
		AddMovementInput(Direction, Value);

	}

	
}

void AZeusCharacter::MoveRight(float Value)
{
	// Controller是从Apwan类继承来的，表示当前控制这个角色的控制器
	if (Controller != nullptr && Value != 0.f)
	{
		
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

		// AddMovementInput 是 ACharacter 类中的一个方法，用于向角色添加移动输入。
		// 这句代码的作用是根据指定的方向和输入值来移动角色。
		// 正值表示向右，负值表示向左
		AddMovementInput(Direction, Value);
	}
}

void AZeusCharacter::Turn(float Value)
{
	// 左右看，参数value控制水平旋转的角度值
	// Value 参数表示旋转的角度值。这个值通常是从输入设备（如鼠标或控制器）获取的。
	// 例如，移动鼠标或推摇杆会产生一个相应的输入值。
	AddControllerYawInput(Value);

	
}

void AZeusCharacter::LookUp(float Value)
{
	// 上下看，控制俯仰视角的角度
	AddControllerPitchInput(Value);
}









void AZeusCharacter::Destroyed()
{
	Super::Destroyed();

	if (ElimBotComponent)
	{
		ElimBotComponent->DestroyComponent();
	}
}

void AZeusCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PollInit();
	/*
	// 每帧检测指针是否为空，因为这个指针只是声明了，没有定义赋值，而定义赋值是在SetOverlappingWeapon函数里的
	// 而这个函数是在weapon类里检测服务器碰撞到才会更新赋值的，
	// 因为tick函数是每个客户端和服务器都会触发的检测，所以这个会导致所有客户端都调用显示文本函数
	// 而我们只需要谁触发就显示在谁客户端上，不是显示在所有客户端上
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	*/
	
	// 如果角色不是模拟代理，那就用服务器通信版的瞄准偏移
	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}

	HideCameraIFCharacterClose();
}


// PlayerInputComponent->BindAxis函数会自动处理输入轴的值并调用绑定的函数。
// 当玩家在游戏中移动鼠标或使用控制器的右摇杆时，Unreal Engine会自动检测这些输入，并将相应的值传递给Turn函数中的Value参数。
// 因此，你不需要手动调用Turn函数或传递参数，Unreal Engine会根据玩家的输入自动完成这些操作。
void AZeusCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 绑定跳跃动作 IE_Pressed：这是一个枚举值，表示输入事件的类型。在这里，IE_Pressed 表示按键被按下时触发。
	// 这里用的是父类自带的jump函数，没有重载
	// 这里第一个参数是编辑器里设置的字符串名称
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AZeusCharacter::Jump);
	// PlayerInputComponent这是一个输入组件，负责处理玩家的输入。
	//  /this代表当前类实例对象的指针，当前类是个角色类，所以控制角色
	// 这是 PlayerInputComponent 的一个方法，用于绑定轴输入。"MoveForward"：这是输入轴的名称，通常在项目设置中定义。
	PlayerInputComponent->BindAxis("MoveForWard", this, &ThisClass::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ThisClass::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ThisClass::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ThisClass::LookUp);

	// 绑定装备武器按钮
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ThisClass::EquipButtonPressed);
	// 绑定下蹲动作
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ThisClass::CrouchButtonPressed);
	// 绑定鼠标右键瞄准动作,一个按下一个释放
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ThisClass::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ThisClass::AimButtonReleased);

	// 绑定鼠标左键开火动作
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ThisClass::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ThisClass::FireButtonReleased);

	PlayerInputComponent->BindAction("Reload", IE_Released, this, &ThisClass::ReloadButtonPressed);
}

// GetLifetimeReplicatedProps用于注册一个类的网络同步属性
// 当服务器需要将某个 Actor 的属性同步到客户端时，这个函数会被调用来确定哪些属性需要同步以及如何同步。
void AZeusCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 将 AZeusCharacter 类中的 OverlappingWeapon 变量注册为需要在网络上进行复制的属性，
	// 以确保在客户端和服务器之间同步该属性的值。
	// DOREPLIFETIME(AZeusCharacter, OverlappingWeapon);
	// COND_OwnerOnly: 这是复制条件，表示该变量只会复制给拥有该角色的客户端。
	DOREPLIFETIME_CONDITION(AZeusCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(AZeusCharacter, Health);
}

void AZeusCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
}


void AZeusCharacter::OnRep_OverlappingWeapon(AWeapon *LastWeapon)
{

	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}


	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
	
}



void AZeusCharacter::EquipButtonPressed()
{
	if (Combat )
	{
		// 有权威的表示服务器端执行的按E键
		if (HasAuthority())
		{
			// NOTE: 只有当角色进入碰撞区域时这个OverlappingWeapon指针才会有值，所以在combat里不用检测武器和角色的碰撞事件
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			// 没权威就是客户端按E了
			ServerEquipButtonPressed();
		}
	}
}





// _Implementation 后缀表示这是在服务器端执行的函数实现.客户端不需要写这个逻辑代码只用调用就行
// 客户端调用 ServerEquipButtonPressed 函数时，Unreal Engine 会将这个调用请求发送到服务器。
// _Implementation 后缀：这个后缀用于定义服务器端的实际逻辑。当客户端调用这个函数时，Unreal Engine 会在服务器上执行带有 _Implementation 后缀的函数。
void AZeusCharacter::ServerEquipButtonPressed_Implementation()
{
	// HasAuthority()不需要检查这个权威，因为这个函数只在服务器执行
	if (Combat)
	{
		// 只有当角色进入碰撞区域时这个OverlappingWeapon指针才会有值，所以在combat里不用检测武器和角色的碰撞事件
		Combat->EquipWeapon(OverlappingWeapon);
	}
}




void AZeusCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{

	// 一开始OverlappingWeapon为空，所以这条不执行，第二次调用时OverlappingWeapon就有值了，
	// 所以这条就执行了，执行后又把OverlappingWeapon设为空了
	
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	
	OverlappingWeapon = Weapon;

	
	// 检查当前 Actor 是否由本地客户端控制，因为主机既是服务器又是客户端
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

bool AZeusCharacter::IsWeaponEquipped()
{
	// 检查是否装备了武器,检查指针是否空指针，因为EquipedWeapon如果不是空代表已经装备了武器
	return (Combat && Combat->EquipedWeapon);
}

bool AZeusCharacter::IsAiming()
{
	// 检查布尔值，是否处于瞄准状态
	return (Combat && Combat->bAiming);
}

AWeapon* AZeusCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr)return nullptr;
	return Combat->EquipedWeapon;
}

FVector AZeusCharacter::GetHitTarget() const
{
	if (Combat == nullptr)return FVector();
	return Combat->HitTarget;
}




void AZeusCharacter::CrouchButtonPressed()
{
	// 检查是否已经是蹲下状态
	if (bIsCrouched)
	{
		UnCrouch();
		// bIsCrouched是否处于蹲下状态，是个布尔值，内置的
		// 而UnCrouch和Crouch是改变这个布尔值的
	}
	else
	{
		Crouch();
	}
}

void AZeusCharacter::ReloadButtonPressed()
{
	if (Combat)
	{
		Combat->Reload();
	}
}

void AZeusCharacter::AimButtonPressed()
{
	// 其实这个战斗组件指针一直都有值
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void AZeusCharacter::AimButtonReleased()
{
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}


float AZeusCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}




void AZeusCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquipedWeapon == nullptr) return;
	FVector Velocity =GetVelocity();
	Velocity.Z = 0.f;
	float  Speed = CalculateSpeed();

	bool bIsInAir = GetCharacterMovement()->IsFalling();

	// 站着不动，不在跳跃状态
	//t 1.当角色移动后停止时，StartingAimRotation值是最后停止移动的那一帧的值，当然也就是鼠标的移动值
	//t 比如角色移动几下停止后，你双手AFK，此时的StartingAimRotation是120.f，鼠标也就是GetBaseAimRotation()，也就是CurrentAimRotation的值也是120.f
	//t 再者因为这个旋转值是根据yaw值去更改动画动作的，你两个值一样，相减就是0yaw,那就是基础姿势，然后鼠标转向，那yaw值就变了动作也变了。很简单
	if (Speed == 0.f && !bIsInAir)
	{
		// 获取当前的鼠标旋转值的yaw值
		bRotateRootBone = true;
		FRotator CurrentAimRotation= FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		// 计算鼠标旋转值和角色移动后停下来的值的差异，也就是yaw值的差值
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation,StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		// 禁用控制器旋转，角色不会跟鼠标转向
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAo_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);		//! 这里这个deltatime参数可以不加，应该是预判会用这个，但是没用到，直接调用这个函数就行
		// UE_LOG(LogTemp, Warning, TEXT("yes start yaw  %f"), StartingAimRotation.Yaw);
		// UE_LOG(LogTemp, Warning, TEXT("asd yaw  %f"),AO_Yaw);
		// UE_LOG(LogTemp, Warning, TEXT("base yaw  %f"), CurrentAimRotation.Yaw);
	}


	if (Speed > 0.f || bIsInAir)
	{
		bRotateRootBone = false;
		// 更新角色移动时或空中时的旋转值。其实只用角色停下来的那一帧,始终获取角色朝向正面的																		// 		为什么差值能表示瞄准偏移
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);																//		当角色停止移动时，StartingAimRotation 记录了角色停止时的瞄准方向。
		// 关闭瞄准偏移																																					//		之后，任何鼠标移动都会改变 CurrentAimRotation，但 StartingAimRotation 保持不变。
		AO_Yaw = 0.f;																																					 	//		通过计算 CurrentAimRotation 和 StartingAimRotation 之间的差值，我们可以得到角色在静止状态下的瞄准偏移量，即 AO_Yaw。
		// 启用控制器旋转，角色会跟鼠标转向																													//		假设角色停止时的 StartingAimRotation 为 30 度（Yaw），此时 AO_Yaw 为 0。
		bUseControllerRotationYaw = true;																													//		如果鼠标移动使 CurrentAimRotation 变为 60 度（Yaw），则 DeltaAimRotation.Yaw 为 60 - 30 = 30 度，AO_Yaw 为 30 度。
		// UE_LOG(LogTemp, Warning, TEXT("no start yaw  %f"), StartingAimRotation.Yaw);
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	
	}																																												//		这表示角色在静止状态下，瞄准方向相对于停止时的方向偏移了 30 度。

	CalculateAO_Pitch();
	
}

void AZeusCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// 定义一个二维向量 InRange，表示输入范围为 270 到 360 度。
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		// 使用 FMath::GetMappedRangeValueClamped 函数将 AO_Pitch 从 InRange 映射到 OutRange。这个函数确保映射后的值在输出范围内。也就是说，
		// 如果 AO_Pitch 在 270 到 360 度之间，它会被映射到 -90 到 0 度之间。
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);;
	}
}

void AZeusCharacter::SimProxiesTurn()
{
	if (Combat == nullptr || Combat->EquipedWeapon == nullptr)return;
	bRotateRootBone = false;
	float Speed = CalculateSpeed();
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}


	bRotateRootBone = false; 

	
	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw=UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_RIght;
		}
		else if(ProxyYaw<-TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

void AZeusCharacter::Jump()
{
	
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}

}


// 本来角色转向到最左边后会突然转到最右边，这样设置一下，角色转向到最左边时会跳到中间也就是角色朝前了
void AZeusCharacter::TurnInPlace(float DeltaTime)
{
	// UE_LOG(LogTemp, Display, TEXT("Ao_yaw:%f"),AO_Yaw);
	if (AO_Yaw > 60.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_RIght;
	}
	else if (AO_Yaw<-90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAo_Yaw = FMath::FInterpTo(InterpAo_Yaw, 0.f, DeltaTime, 5.f);
		AO_Yaw = InterpAo_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

/*
void AZeusCharacter::ServerHit_Implementation()
{
	MulticastHit();
}


void AZeusCharacter::MulticastHit_Implementation()
{
	PlayHitReactMontage();
}
*/

void AZeusCharacter::HideCameraIFCharacterClose()
{

	// 检查是否为本地控制的角色，如果不是，则直接返回不执行后续操作。这用于确保只有本地控制的角色执行可见性更改
	if (!IsLocallyControlled())return;

	// (FollowCamera->GetComponentLocation() - GetActorLocation()).Size() 计算摄像机和角色的位置距离。
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquipedWeapon && Combat->EquipedWeapon->GetWeaponMesh())
		{
			// bOwnerNoSee 设置为 true，表示角色自己看不到武器网格。
			Combat->EquipedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquipedWeapon && Combat->EquipedWeapon->GetWeaponMesh())
		{
			Combat->EquipedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void AZeusCharacter::FireButtonPressed()
{
	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void AZeusCharacter::FireButtonReleased()
{
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}



void AZeusCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquipedWeapon == nullptr) return;

	// 这行代码获取角色的动画实例（UAnimInstance），用于控制动画的播放。
	// 动画实例是用来播放复杂的动画序列，简单的动画可以直接用骨骼组件调用playanimantion
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	// 检查 AnimInstance 和 FireWeaponMontage 是否有效。如果有效，则播放 FireWeaponMontage 动画蒙太奇。
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		// 这段代码根据 bAiming 参数选择要播放的动画段。如果 bAiming 为 true，则选择 "RifleAim" 段；
		// 否则选择 "RifleHip" 段。然后，使用 Montage_JumpToSection 函数跳转到指定的动画段。
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AZeusCharacter::PlayHitReactMontage()
{

	if (Combat == nullptr || Combat->EquipedWeapon == nullptr) return;

	// 这行代码获取角色的动画实例（UAnimInstance），用于控制动画的播放。
	// 动画实例是用来播放复杂的动画序列，简单的动画可以直接用骨骼组件调用playanimantion
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	// 检查 AnimInstance 和 FireWeaponMontage 是否有效。如果有效，则播放 FireWeaponMontage 动画蒙太奇。
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");
		// 这段代码根据 bAiming 参数选择要播放的动画段。如果 bAiming 为 true，则选择 "RifleAim" 段；
		// 否则选择 "RifleHip" 段。然后，使用 Montage_JumpToSection 函数跳转到指定的动画段。
		
		AnimInstance->Montage_JumpToSection(SectionName);
	}

	
}




void AZeusCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	 TimeSinceLastMovementReplication = 0.f;
}




void AZeusCharacter::OnRep_Health()
{

	UpdateHUDHealth();
	
	// 手机动画有问题bug
	PlayHitReactMontage();

	
}

void AZeusCharacter::ReceiveDamage(AActor* DamgedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{

	
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	UpdateHUDHealth();
	

	

	if (Health == 0.f)
	{

		

		// GetAuthGameMode<AZeusGameMode>() 获取当前世界的 GameMode 实例
		AZeusGameMode* ZeusGameMode = GetWorld()->GetAuthGameMode<AZeusGameMode>();
		if (ZeusGameMode)
		{

			

			// 角色控制器
			ZeusPlayerController = ZeusPlayerController == nullptr ? Cast<AZeusPlayerController>(Controller) : ZeusPlayerController;
			// 攻击者的角色控制器
			AZeusPlayerController* AttackerController = Cast < AZeusPlayerController>(InstigatorController);
			ZeusGameMode->PlayerEliminated(this, ZeusPlayerController, AttackerController);
		}
	}
	else
	{
		PlayHitReactMontage();
	}
	
}

void AZeusCharacter::Elim()
{
	if (Combat && Combat->EquipedWeapon)
	{
		Combat->EquipedWeapon->Dropped();
	}
	MulticastElim();
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&AZeusCharacter::ElimTimerFinished,
		ElimDelay);
}



void AZeusCharacter::MulticastElim_Implementation()
{
	if (ZeusPlayerController)
	{
		ZeusPlayerController->SetHUDAmmo(0);
	}

	bElimmed = true;
	PlayElimMontage();

	// 开始溶解特效
	// 检查 DissolveMaterialInstance 是否有效，即是否已经分配了基础材质实例。新创建的动态材质实例将基于这个基础材质实例。
	if (DissolveMaterialInstance)
	{
		// 动态材质实例允许在运行时修改材质参数，从而实现动态效果。
		// this 参数将新创建的动态材质实例与 AZeusCharacter 实例关联。这种关联确保动态材质实例的生命周期与角色实例的生命周期一致。
		// 通过将动态材质实例与角色实例关联，Unreal Engine 可以更好地管理和释放资源。当角色实例被销毁时，相关联的动态材质实例也会被适当地清理，避免资源泄漏。
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);

		// 将角色的网格材质设置为动态材质实例，以应用溶解效果。
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);
		// 设置材质参数，控制溶解效果的程度。
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), -0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 200.f);
	}
	StartDissolve();

	// 关闭角色移动
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	// 关闭角色输入
	if (ZeusPlayerController)
	{
		DisableInput(ZeusPlayerController);
	}
	// 关闭碰撞
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 生成淘汰特效
	// 如果设置了粒子特效
	if (ElimBotEffect)
	{
		// 创建一个位置向量，在角色头上200.f处
		FVector ElimBotSpawnPont(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 200.f);
		// UGameplayStatics的静态函数在指定位置生成粒子特效
		ElimBotComponent=UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), ElimBotEffect, ElimBotSpawnPont,GetActorRotation()
		);
	}

	if (ElimBotSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this, ElimBotSound, GetActorLocation()
		);
	}
}

void AZeusCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
		
	}
}

void AZeusCharacter::ElimTimerFinished()
{
	AZeusGameMode* ZeusGameMode = GetWorld()->GetAuthGameMode<AZeusGameMode>();
	if (ZeusGameMode)
	{
		ZeusGameMode->RequestRespawn(this, Controller);
	}

	// 销毁粒子效果
	if (ElimBotComponent)
	{
		ElimBotComponent->DestroyComponent();
	}
}


void AZeusCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

void AZeusCharacter::StartDissolve()
{
	// 委托类型，绑定事件，将 UpdateDissolveMaterial 函数绑定到 DissolveTrack 事件上，以便时间轴更新时调用该函数
	DissolveTrack.BindDynamic(this, &AZeusCharacter::UpdateDissolveMaterial);
	// DissolveCurve 这是一个指向浮点曲线的指针。浮点曲线用于定义在时间轴上的数值变化。
	if (DissolveCurve&& DissolveTimeline)
	{
		// DissolveTimeline这是一个时间轴组件，控制动画效果的时间进程。
		// AddInterpFloat将浮点插值曲线（DissolveCurve）和对应的事件（DissolveTrack）添加到时间轴中，使时间轴能够控制曲线的数值变化。
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		// 启动时间轴的播放。时间轴将根据曲线定义的数值变化，逐帧更新绑定的事件。
		DissolveTimeline->Play();
	}
}



void AZeusCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquipedWeapon == nullptr) return;

	// 这行代码获取角色的动画实例（UAnimInstance），用于控制动画的播放。
	// 动画实例是用来播放复杂的动画序列，简单的动画可以直接用骨骼组件调用playanimantion
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	// 检查 AnimInstance 和 FireWeaponMontage 是否有效。如果有效，则播放 FireWeaponMontage 动画蒙太奇。
	if (AnimInstance &&ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		// 这里的SectionName要用蒙太奇动画的蓝色的片段名称
		FName SectionName;
		switch (Combat->EquipedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName);
	}

}


ECombatState AZeusCharacter::GetCombatState() const
{
	if (Combat == nullptr) return ECombatState::ECS_MAX;
	return Combat->CombatState;
}
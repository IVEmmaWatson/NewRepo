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
}


void AZeusCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AZeusCharacter::MoveForward(float Value)
{
	// Controller是从Apwan类继承来的，表示当前控制这个角色的控制器
	if (Controller != nullptr && Value != 0.f)
	{
		// Controller->GetControlRotation().Yaw获取玩家控制器的当前控制旋转的 Yaw 值。
		// 以当前yaw值、忽略其他的值创建一个旋转类对象，因为yaw代表玩家的水平旋转也就是角色朝向，把其他值固定住
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		// FRotationMatrix(YawRotation)：使用 YawRotation 创建一个旋转矩阵。
		// GetUnitAxis(EAxis::X)：从旋转矩阵中获取 X 轴的单位向量。
		// FVector Direction：将结果存储在一个 FVector 对象中，表示方向向量。因为X轴始终表示角色正前方
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));

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
	AddControllerYawInput(Value);
}

void AZeusCharacter::LookUp(float Value)
{
	// 上下看，控制俯仰视角的角度
	AddControllerPitchInput(Value);
}

void AZeusCharacter::EquipButtonPressed()
{
	if (Combat&&HasAuthority())
	{
		// 只有当角色进入碰撞区域时这个OverlappingWeapon指针才会有值，所以在combat里不用检测武器和角色的碰撞事件
		Combat->EquipWeapon(OverlappingWeapon);
	}
}


void AZeusCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
}


void AZeusCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 绑定跳跃动作 IE_Pressed：这是一个枚举值，表示输入事件的类型。在这里，IE_Pressed 表示按键被按下时触发。
	// 这里用的是父类自带的jump函数，没有重载
	// 这里第一个参数是编辑器里设置的字符串名称
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	// PlayerInputComponent这是一个输入组件，负责处理玩家的输入。
	// this代表当前类实例对象的指针，当前类是个角色类，所以控制角色
	// 这是 PlayerInputComponent 的一个方法，用于绑定轴输入。"MoveForward"：这是输入轴的名称，通常在项目设置中定义。
	PlayerInputComponent->BindAxis("MoveForWard", this, &ThisClass::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ThisClass::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ThisClass::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ThisClass::LookUp);

	// 绑定装备武器按钮
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ThisClass::EquipButtonPressed);
}

// GetLifetimeReplicatedProps用于注册一个类的网络同步属性
// 当服务器需要将某个 Actor 的属性同步到客户端时，这个函数会被调用来确定哪些属性需要同步以及如何同步。
void AZeusCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 将 AZeusCharacter 类中的 OverlappingWeapon 属性注册为需要在网络上进行复制的属性，
	// 以确保在客户端和服务器之间同步该属性的值。
	// DOREPLIFETIME(AZeusCharacter, OverlappingWeapon);
	// COND_OwnerOnly: 这是复制条件，表示该变量只会复制给拥有该角色的客户端。
	DOREPLIFETIME_CONDITION(AZeusCharacter, OverlappingWeapon, COND_OwnerOnly);
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



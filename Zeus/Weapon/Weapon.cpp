// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Zeus\Character\ZeusCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Zeus/ZeusComponents/CombatComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AWeapon::AWeapon()
{

	// 启用Actor或组件的网络复制功能,启用这个weapon组件的的网络复制
	bReplicates = true;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// 这行代码用于启用Actor的Tick功能，使其每帧都能执行特定的逻辑。就是每帧都执行下面的tick函数
	PrimaryActorTick.bCanEverTick = false;

	// 创建骨骼网格组件实例对象
	// 你创建了一个actor蓝图，然后你得创建骨骼体和网格体，然后吧这个绑定到这个actor类的根组件上
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	// WeaponMesh->SetupAttachment(RootComponent);
	SetRootComponent(WeaponMesh);
	// SetCollisionResponseToAllChannels：这是一个函数，用于设置组件对所有碰撞通道的响应方式
	// ECollisionResponse::ECR_Block：这是一个枚举值，表示碰撞响应类型为“阻挡”。
	// ECR_Block：表示当其他物体与WeaponMesh发生碰撞时，会被阻挡，无法穿过该组件。这通常用于需要物理交互的物体，例如武器、墙壁等。
	// ECR_Ignore：忽略碰撞，不会产生任何物理反应。ECR_Overlap：产生重叠事件，但不阻挡物体，可以用于触发器等。
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	// 对特定的组件类型响应方式改为忽略碰撞，这个特定类型是pawn是玩家角色
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	// ECollisionEnabled::NoCollision：禁用所有碰撞检测，物体将不会与其他物体发生碰撞。
	// SetCollisionEnabled：这是一个函数，用于设置组件的碰撞启用状态。
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 创建一个区域触发器，当玩家进入这个球形区域空间时会被检测到，然后执行拿枪或其他逻辑
	// 创建一个球形的碰撞检测组件，并实例化
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	// 这行代码将AreaSphere组件对所有碰撞通道的响应设置为“忽略”（Ignore）。这意味着AreaSphere不会与任何其他物体发生碰撞
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	// 这行代码禁用AreaSphere的所有碰撞检测。设置为NoCollision后，AreaSphere将不会与任何物体进行物理交互
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	// 创建一个用户控件实例对象,绑定到根组件
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);

	FireAnimation = CreateDefaultSubobject<UAnimationAsset>(TEXT("FireAnimation"));
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	// 一般情况下HasAuthority()服务器端返回true,客户端返回false,当listen模式下，主机和服务器都返回true,其他客户端返回false
	// 所以是只在服务器里检测碰撞，只有服务器里发生碰撞才会有效然后反馈给客户端，防止作弊
	if (HasAuthority())
	{
		// 开启开启物理碰撞交互检测
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		// 设置该碰撞组件对玩家角色的响应设置为重叠。这意味着当玩家角色进入AreaSphere的碰撞区域时，会触发重叠事件，但不会阻挡玩家角色的移动。
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		// OnComponentBeginOverlap是一个动态多播委托 AddDynamic绑定委托回调函数
		// 当 AreaSphere 组件检测到有其他组件开始与它重叠时，调用当前对象的 OnSphereOverlap 函数来处理这个事件
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereEndOverlap);
	}

	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
}

// 检测玩家碰撞重叠后的回调函数
void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OhterActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// cast将一个父类转换成子类类型
	AZeusCharacter* ZeusCharacter = Cast<AZeusCharacter>(OhterActor);
	// 如果指针有效
	if (ZeusCharacter)
	{
		// 调用控件设置显示为true
		// 这句代码的作用是将 PickupWidget 的可见性设置为 true，即使这个UI组件在游戏中可见。
		// PickupWidget->SetVisibility(true);
		ZeusCharacter->SetOverlappingWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OhterActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex )
{
	AZeusCharacter* ZeusCharacter = Cast<AZeusCharacter>(OhterActor);
	// 如果指针有效
	if (ZeusCharacter)
	{
		// 调用控件设置显示为true
		// 这句代码的作用是将 PickupWidget 的可见性设置为 true，即使这个UI组件在游戏中可见。
		// PickupWidget->SetVisibility(true);
		// 更改了overlappingweapon的值，因为如果碰撞了这个值是这个类的实例指针，是有正常值的，现在又改变了值，所有又调用了其他函数
		ZeusCharacter->SetOverlappingWeapon(nullptr);
	}
}


// 这个主要用于客户端检测武器状态
void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	// // 隐藏捡起武器文本

	switch (WeaponState)
	{
	case EWeaponState::EWS_Equiped:
		ShowPickupWidget(false);
		// 关闭碰撞检测
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	
	}
	
}



// 这个主要用于服务器检测武器状态
void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Initial:
		break;
	case EWeaponState::EWS_Equiped:
		// 关闭按E文本显示，并取消碰撞区域，将碰撞检测关闭
		ShowPickupWidget(false);
		// AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped:
		break;
	case EWeaponState::EWS_MAX:
		break;
	default:
		break;
	}
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 将这个类的WeaponState变量注册为在网络上可复制
	DOREPLIFETIME(AWeapon, WeaponState);
}

void AWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void AWeapon::Fire(const FVector& HitTarget)
{
	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation,false);
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);

	//? ECC_WorldDynamic 是其中一个通道，通常用于动态的世界物体。动态物体是指那些在游戏中可以移动或改变状态的物体，例如角色、车辆、可破坏的物体等。
	// 设置子弹物理类型表示这是一个可移动或改变形状的物体
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	//? 启动物理碰撞检测
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//? 将所有通道的碰撞响应事件改为忽略碰撞
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//? 设置特定通道的响应事件为阻挡，ECC_Visibility视线通道，ECC_WorldStatic静态世界物体（如墙壁、地面）
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

	//t ProjectileMovementComponent这是一个用于管理弹丸运动的组件。它负责处理弹丸的移动逻辑，比如速度、重力、阻力等。
	//t bRotationFollowsVelocity：这是一个布尔值属性。设置为 true 后，弹丸的旋转将自动跟随它的速度方向。速度是一个矢量量，包含两个重要的部分：大小和方向。
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


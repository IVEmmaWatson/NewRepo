// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/BoxComponent.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);

	//? ECC_WorldDynamic 是其中一个通道，通常用于动态的世界物体。动态物体是指那些在游戏中可以移动或改变状态的物体，例如角色、车辆、可破坏的物体等。
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	//? 启动物理碰撞检测
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//? 将所有通道的碰撞响应事件改为忽略碰撞
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//? 设置特定通道的响应事件为阻挡，ECC_Visibility视线通道，ECC_WorldStatic静态世界物体（如墙壁、地面）
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
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


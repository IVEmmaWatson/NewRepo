// Fill out your copyright notice in the Description page of Project Settings.
#include "Casing.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"

// Sets default values
ACasing::ACasing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);

	// 忽略相机对蛋壳的碰撞响应，不然相机会放大蛋壳视角
	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	// 开启蛋壳的物理模拟，当需要让物体遵循物理定律，例如自由落体、碰撞反弹时，需要启用物理模拟。
	CasingMesh->SetSimulatePhysics(true);
	//  启用了重力效果。在需要物体受重力影响时，例如投掷物、掉落物体等，启用重力可以增强物理表现的真实性
	CasingMesh->SetEnableGravity(true);
	// SetNotifyRigidBodyCollision(true) 使得 CasingMesh 在发生物理碰撞时可以生成通知，从而触发 OnComponentHit 等事件。
	// 如果不设置这个不会触发 OnComponentHit 事件，这意味着任何与 CasingMesh 的碰撞都不会调用你绑定的 OnHit 函数。
	// 子弹不需要启用刚体碰撞通知，因为其主要任务是命中目标。
	CasingMesh->SetNotifyRigidBodyCollision(true);
	ShellEjectionImpulse = 5.f;
	
}

// Called when the game starts or when spawned
void ACasing::BeginPlay()
{
	Super::BeginPlay();
	
	// 绑定蛋壳被击中事件,当组件发生碰撞时触发
	CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);
	
	// 这行代码为 蛋壳实例 添加一个沿着角色前方向的冲力。
	// GetActorForwardVector(): 这个函数返回当前Actor的前方向向量，ShellEjectionImpulse: 代表初始冲力的大小。
	// 将前方向向量乘以初始冲力，得到一个具体的冲力向量。
	// AddImpulse(): 这是一个 UPrimitiveComponent 的方法，用于为物体添加瞬时冲力。
	CasingMesh->AddImpulse(GetActorForwardVector()*ShellEjectionImpulse);
}

void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OhterActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
	}
	Destroy();
}





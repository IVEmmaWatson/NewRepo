// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Zeus/Character/ZeusCharacter.h"
#include "Sound/SoundCue.h"
#include "Zeus/Zeus.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// 当你在 AProjectile 类中启用 bReplicates = true 后：
	//t生成同步：服务器在生成子弹时，Unreal Engine会自动将这个子弹实例同步到所有客户端。
	//t状态同步：子弹的状态（位置、旋转、粒子特效等）会在服务器和客户端之间保持同步。
	//t视觉效果同步：由于粒子特效是附加到子弹上的，客户端在接收到子弹实例时，也会自动生成和渲染与子弹关联的粒子特效。
	//
	bReplicates = true;

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
	// 设置子弹对骨骼体的碰撞为阻挡
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);
	//t ProjectileMovementComponent这是一个用于管理弹丸运动的组件。它负责处理弹丸的移动逻辑，比如速度、重力、阻力等。
	//t bRotationFollowsVelocity：这是一个布尔值属性。设置为 true 后，弹丸的旋转将自动跟随它的速度方向。速度是一个矢量量，包含两个重要的部分：大小和方向。
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;


}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	// 检查粒子系统在蓝图是否有效，是否被赋值，这个在子弹蓝图里设置
	if (Tracer)
	{
		// 这是生成并附加粒子特效的函数。Tracer：这是要生成的粒子特效，即 UParticleSystem。
		// CollisionBox：这是粒子特效附加的组件。在这个例子中是 CollisionBox，也就是子弹的碰撞盒。
		// FName()：这是附加到组件上的插槽名称，这里使用默认名称。即便子弹actor没有插槽，FName() 依然可以使用默认值来附加到指定组件上。这意味着粒子特效会附加到 CollisionBox 的原点位置。
		// GetActorLocation() 函数返回当前Actor在世界坐标系中的位置。这表示粒子特效将会在当前Actor所在的位置生成。
		// GetActorRotation() 函数返回当前Actor在世界坐标系中的旋转。这表示粒子特效将会按当前Actor的旋转方向生成。
		// EAttachLocation::KeepWorldPosition：这个枚举值指定附加时保持当前的世界位置和旋转。简单来说，粒子特效将会在生成时保持传递给它的世界位置和旋转，而不是基于附加的组件位置和旋转。
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(
			Tracer,
			CollisionBox,
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition
		);

		
	}

	if (HasAuthority())
	{
		// 绑定碰撞检测事件，是个委托事件，当这个子弹盒被其他对象撞击时触发
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	}

}

// 
void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OhterActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AZeusCharacter* ZeusCharacter = Cast<AZeusCharacter>(OhterActor);

	if (ZeusCharacter)
	{
		ZeusCharacter->ServerHit();
	}





	// 播放特效
	// 销毁这个acotr对象，调用后这个Actor将被标记为待销毁，并在下一帧中实际销毁。
	// 由于 Destroyed() 方法会在所有网络端调用，当服务器调用 Destroy() 后，客户端也会执行相应的 Destroyed() 方法，从而播放特效和声音。
	Destroy();
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Destroyed() 是一个生命周期回调函数，当Actor实际被销毁时会被调用。这个函数在 Destroy() 标记对象待销毁并实际移除对象时触发。
// 它会在所有网络端（服务器和客户端）都被调用，使得效果可以同步显示
void AProjectile::Destroyed()
{
	Super::Destroyed();

	if (ImpactParticles)
	{
		// SpawnEmitterAtLocation：在指定位置生成粒子效果的函数。
		// GetActorTransform获取当前Actor的位置、旋转和缩放信息，用于在正确的位置生成粒子效果。
		// GetWorld()：获取当前世界的指针，这个指针用于确定在哪个世界中生成粒子效果。ImpactParticles：指定的粒子效果资源。
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());

	}

	// 播放声音
	if (ImpactSound)
	{
		// 一发子弹就是一个AProjectile实例对象，this指向当前发射的这发子弹的实例
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
}



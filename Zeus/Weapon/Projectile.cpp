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
	// ������ AProjectile �������� bReplicates = true ��
	//t����ͬ�����������������ӵ�ʱ��Unreal Engine���Զ�������ӵ�ʵ��ͬ�������пͻ��ˡ�
	//t״̬ͬ�����ӵ���״̬��λ�á���ת��������Ч�ȣ����ڷ������Ϳͻ���֮�䱣��ͬ����
	//t�Ӿ�Ч��ͬ��������������Ч�Ǹ��ӵ��ӵ��ϵģ��ͻ����ڽ��յ��ӵ�ʵ��ʱ��Ҳ���Զ����ɺ���Ⱦ���ӵ�������������Ч��
	//
	bReplicates = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);

	//? ECC_WorldDynamic ������һ��ͨ����ͨ�����ڶ�̬���������塣��̬������ָ��Щ����Ϸ�п����ƶ���ı�״̬�����壬�����ɫ�����������ƻ�������ȡ�
	// �����ӵ��������ͱ�ʾ����һ�����ƶ���ı���״������
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	//? ����������ײ���
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//? ������ͨ������ײ��Ӧ�¼���Ϊ������ײ
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//? �����ض�ͨ������Ӧ�¼�Ϊ�赲��ECC_Visibility����ͨ����ECC_WorldStatic��̬�������壨��ǽ�ڡ����棩
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	// �����ӵ��Թ��������ײΪ�赲
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);
	//t ProjectileMovementComponent����һ�����ڹ������˶��������������������ƶ��߼��������ٶȡ������������ȡ�
	//t bRotationFollowsVelocity������һ������ֵ���ԡ�����Ϊ true �󣬵������ת���Զ����������ٶȷ����ٶ���һ��ʸ����������������Ҫ�Ĳ��֣���С�ͷ���
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;


}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	// �������ϵͳ����ͼ�Ƿ���Ч���Ƿ񱻸�ֵ��������ӵ���ͼ������
	if (Tracer)
	{
		// �������ɲ�����������Ч�ĺ�����Tracer������Ҫ���ɵ�������Ч���� UParticleSystem��
		// CollisionBox������������Ч���ӵ������������������� CollisionBox��Ҳ�����ӵ�����ײ�С�
		// FName()�����Ǹ��ӵ�����ϵĲ�����ƣ�����ʹ��Ĭ�����ơ������ӵ�actorû�в�ۣ�FName() ��Ȼ����ʹ��Ĭ��ֵ�����ӵ�ָ������ϡ�����ζ��������Ч�ḽ�ӵ� CollisionBox ��ԭ��λ�á�
		// GetActorLocation() �������ص�ǰActor����������ϵ�е�λ�á����ʾ������Ч�����ڵ�ǰActor���ڵ�λ�����ɡ�
		// GetActorRotation() �������ص�ǰActor����������ϵ�е���ת�����ʾ������Ч���ᰴ��ǰActor����ת�������ɡ�
		// EAttachLocation::KeepWorldPosition�����ö��ֵָ������ʱ���ֵ�ǰ������λ�ú���ת������˵��������Ч����������ʱ���ִ��ݸ���������λ�ú���ת�������ǻ��ڸ��ӵ����λ�ú���ת��
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
		// ����ײ����¼����Ǹ�ί���¼���������ӵ��б���������ײ��ʱ����
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





	// ������Ч
	// �������acotr���󣬵��ú����Actor�������Ϊ�����٣�������һ֡��ʵ�����١�
	// ���� Destroyed() ����������������˵��ã������������� Destroy() �󣬿ͻ���Ҳ��ִ����Ӧ�� Destroyed() �������Ӷ�������Ч��������
	Destroy();
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Destroyed() ��һ���������ڻص���������Actorʵ�ʱ�����ʱ�ᱻ���á���������� Destroy() ��Ƕ�������ٲ�ʵ���Ƴ�����ʱ������
// ��������������ˣ��������Ϳͻ��ˣ��������ã�ʹ��Ч������ͬ����ʾ
void AProjectile::Destroyed()
{
	Super::Destroyed();

	if (ImpactParticles)
	{
		// SpawnEmitterAtLocation����ָ��λ����������Ч���ĺ�����
		// GetActorTransform��ȡ��ǰActor��λ�á���ת��������Ϣ����������ȷ��λ����������Ч����
		// GetWorld()����ȡ��ǰ�����ָ�룬���ָ������ȷ�����ĸ���������������Ч����ImpactParticles��ָ��������Ч����Դ��
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());

	}

	// ��������
	if (ImpactSound)
	{
		// һ���ӵ�����һ��AProjectileʵ������thisָ��ǰ������ⷢ�ӵ���ʵ��
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
}



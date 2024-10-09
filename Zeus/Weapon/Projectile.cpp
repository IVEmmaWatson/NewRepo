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

	//t ProjectileMovementComponent����һ�����ڹ������˶��������������������ƶ��߼��������ٶȡ������������ȡ�
	//t bRotationFollowsVelocity������һ������ֵ���ԡ�����Ϊ true �󣬵������ת���Զ����������ٶȷ����ٶ���һ��ʸ����������������Ҫ�Ĳ��֣���С�ͷ���
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


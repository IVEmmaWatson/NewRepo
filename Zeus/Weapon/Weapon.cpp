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

	// ����Actor����������縴�ƹ���,�������weapon����ĵ����縴��
	bReplicates = true;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// ���д�����������Actor��Tick���ܣ�ʹ��ÿ֡����ִ���ض����߼�������ÿ֡��ִ�������tick����
	PrimaryActorTick.bCanEverTick = false;

	// ���������������ʵ������
	// �㴴����һ��actor��ͼ��Ȼ����ô���������������壬Ȼ�������󶨵����actor��ĸ������
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	// WeaponMesh->SetupAttachment(RootComponent);
	SetRootComponent(WeaponMesh);
	// SetCollisionResponseToAllChannels������һ���������������������������ײͨ������Ӧ��ʽ
	// ECollisionResponse::ECR_Block������һ��ö��ֵ����ʾ��ײ��Ӧ����Ϊ���赲����
	// ECR_Block����ʾ������������WeaponMesh������ײʱ���ᱻ�赲���޷��������������ͨ��������Ҫ�����������壬����������ǽ�ڵȡ�
	// ECR_Ignore��������ײ����������κ�����Ӧ��ECR_Overlap�������ص��¼��������赲���壬�������ڴ������ȡ�
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	// ���ض������������Ӧ��ʽ��Ϊ������ײ������ض�������pawn����ҽ�ɫ
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	// ECollisionEnabled::NoCollision������������ײ��⣬���彫�������������巢����ײ��
	// SetCollisionEnabled������һ�����������������������ײ����״̬��
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// ����һ�����򴥷���������ҽ��������������ռ�ʱ�ᱻ��⵽��Ȼ��ִ����ǹ�������߼�
	// ����һ�����ε���ײ����������ʵ����
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	// ���д��뽫AreaSphere�����������ײͨ������Ӧ����Ϊ�����ԡ���Ignore��������ζ��AreaSphere�������κ��������巢����ײ
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	// ���д������AreaSphere��������ײ��⡣����ΪNoCollision��AreaSphere���������κ��������������
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	// ����һ���û��ؼ�ʵ������,�󶨵������
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);

	FireAnimation = CreateDefaultSubobject<UAnimationAsset>(TEXT("FireAnimation"));
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	// һ�������HasAuthority()�������˷���true,�ͻ��˷���false,��listenģʽ�£������ͷ�����������true,�����ͻ��˷���false
	// ������ֻ�ڷ�����������ײ��ֻ�з������﷢����ײ�Ż���ЧȻ�������ͻ��ˣ���ֹ����
	if (HasAuthority())
	{
		// ��������������ײ�������
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		// ���ø���ײ�������ҽ�ɫ����Ӧ����Ϊ�ص�������ζ�ŵ���ҽ�ɫ����AreaSphere����ײ����ʱ���ᴥ���ص��¼����������赲��ҽ�ɫ���ƶ���
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		// OnComponentBeginOverlap��һ����̬�ಥί�� AddDynamic��ί�лص�����
		// �� AreaSphere �����⵽�����������ʼ�����ص�ʱ�����õ�ǰ����� OnSphereOverlap ��������������¼�
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereEndOverlap);
	}

	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
}

// ��������ײ�ص���Ļص�����
void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OhterActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// cast��һ������ת������������
	AZeusCharacter* ZeusCharacter = Cast<AZeusCharacter>(OhterActor);
	// ���ָ����Ч
	if (ZeusCharacter)
	{
		// ���ÿؼ�������ʾΪtrue
		// ������������ǽ� PickupWidget �Ŀɼ�������Ϊ true����ʹ���UI�������Ϸ�пɼ���
		// PickupWidget->SetVisibility(true);
		ZeusCharacter->SetOverlappingWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OhterActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex )
{
	AZeusCharacter* ZeusCharacter = Cast<AZeusCharacter>(OhterActor);
	// ���ָ����Ч
	if (ZeusCharacter)
	{
		// ���ÿؼ�������ʾΪtrue
		// ������������ǽ� PickupWidget �Ŀɼ�������Ϊ true����ʹ���UI�������Ϸ�пɼ���
		// PickupWidget->SetVisibility(true);
		// ������overlappingweapon��ֵ����Ϊ�����ײ�����ֵ��������ʵ��ָ�룬��������ֵ�ģ������ָı���ֵ�������ֵ�������������
		ZeusCharacter->SetOverlappingWeapon(nullptr);
	}
}


// �����Ҫ���ڿͻ��˼������״̬
void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	// // ���ؼ��������ı�

	switch (WeaponState)
	{
	case EWeaponState::EWS_Equiped:
		ShowPickupWidget(false);
		// �ر���ײ���
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	
	}
	
}



// �����Ҫ���ڷ������������״̬
void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Initial:
		break;
	case EWeaponState::EWS_Equiped:
		// �رհ�E�ı���ʾ����ȡ����ײ���򣬽���ײ���ر�
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

	// ��������WeaponState����ע��Ϊ�������Ͽɸ���
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
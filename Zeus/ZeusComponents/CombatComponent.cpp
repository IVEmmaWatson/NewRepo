// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Zeus/Weapon/Weapon.h"
#include "Zeus/Character/ZeusCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"


#define TRACE_LENGTH 80000.f
// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	// ����Ҫÿ֡�����£���Ϊ���ǰ������������ϵĶ���
	PrimaryComponentTick.bCanEverTick = true;


	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;
	// ...
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquipedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
}

// �������ֻ���ڷ�������ִ��
void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	// ����ɫָ�������ָ���Ƿ�Ϊ��
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	 
	EquipedWeapon = WeaponToEquip;

	EquipedWeapon->SetWeaponState(EWeaponState::EWS_Equiped);
	// �ӽ�ɫ������ȡ�ֲ����
	// AttachActor�����������һ�� Actor ���ӵ�����ϡ�
	// ��һ��������Ҫ���ӵ� Actor��������������� EquipedWeapon�����ڶ���������Ҫ���ӵ��Ĺ�������
	const USkeletalMeshSocket* HandSocket=Character->GetMesh()->GetSocketByName(FName("righthandsocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(EquipedWeapon,Character->GetMesh());
	}
	// �����������ʵ����ӵ����
	EquipedWeapon->SetOwner(Character);
	
	// ���������Ϊ false ʱ����ɫ��������ƶ���������������ĳ���
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	// ���ý�ɫ���ſ�������ת�����������Ϊ trueʱ����ɫ����ݿ���������ת����������ĳ���
	Character->bUseControllerRotationYaw = true;
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...���ý�ɫ�ƶ��ٶ�
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed= BaseWalkSpeed;
	}

	
	
}

// ͨ�����������Ƿ�����׼���ж��Ƿ��Ƿ�������ɫ������ǿͻ��˽�ɫ�����rpc����
// ��Ϊ�ͻ����Ǹ��ƽ�ɫû�����������ʵ�ʽ�ɫ����ͨ��ֻ���ڱ��ؿ����Լ�������
// ���Ե��ͻ��˰�����׼����Ҫ֪ͨ��������ɫҲ������׼
void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming; // ������ʹ�ͻ��˰��¾Ϳ��Կ�����׼���ƣ���Ȼ��Ҫ�������rpc�������ú���ܿ�������
	if (!Character->HasAuthority())
	{
		ServerSetAiming(bIsAiming);
	}
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;


	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

// ��EquipedWeapon���������仯ʱ���ô˺����ı���ҵ���ת����
// ��Ҫ�������������ʱ����ɫ�ƶ������������ƣ���������������һ��ʼ�ճ�ǰ�����൱�ڵ�һ�˳�ģʽ
void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquipedWeapon && Character)
	{
		// ���������Ϊ false ʱ����ɫ��������ƶ���������������ĳ���
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		// ���ý�ɫ���ſ�������ת�����������Ϊ trueʱ����ɫ����ݿ���������ת����������ĳ���
		Character->bUseControllerRotationYaw = true;
	}
}

// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// HitResult�����洢�������е�Ŀ����Ϣ
	FHitResult HitResult;
	TraceUnderCorsshairs(HitResult);
	
	
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed; 
	if (bFireButtonPressed)
	{
		ServerFire();
	}
}

//t FHitResult��һ�����������߼�����е���ϸ��Ϣ����Ա����ImpactPoint�����е�λ�á�Normal�����е�ķ��ߡ�Actor�����е� Actor��Component�����е������
// TraceHitResult����Ҫ�ֶ�����ʹ���
// _TraceUnderCorsshairs����ͨ����������Ϸ�н������߼��
void UCombatComponent::TraceUnderCorsshairs(FHitResult& TraceHitResult)
{
	// ��ȡ��Ϸ��Ļ��ǰ�ĳߴ�
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// ��ȡ��Ϸ��Ļ�м������
	FVector2D CrosshariLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	FVector CrosshariWorldPosition;	// ���� WorldPosition ��ֵ�� (1000, 500, 200)�����ʾ��Ļ�ϵĵ� (100, 200) ��3D�����еľ���λ���� (1000, 500, 200)��
	FVector CrosshariWorldDirection;	// ���� WorldDirection ��ֵ�� (0.707, 0.707, 0)�����ʾ�����λ�õ� WorldPosition �ķ�����������������ǵ�λ��������ʾ��������Ǿ����λ�á�
	// ��һ����ά����ת������ά���꣬����1��ҿ�����ָ�룬����2Ҫת���Ķ�ά���꣬����3ת���������ռ�3D���꣬����4ת���������ռ䷽��
	bool bScreenToWorld=UGameplayStatics::DeprojectScreenToWorld(
		PlayerController,CrosshariLocation, CrosshariWorldPosition, CrosshariWorldDirection
	);

	
	// UGameplayStatics::GetPlayerController(this, 0);
	if (bScreenToWorld)
	{
		// ��ʼλ�ã�����Ļ�м�����άλ�����꣬һ��ʱ�����������
		// ��Ļ���ĵ��λ�ã�CrosshariWorldPosition��ͨ���ӽ����������λ�ã���Ϊ���Ǵ�������ӽ�ת�������ġ�
		// ���� DeprojectScreenToWorld ��������Ļ���ĵ�ͶӰ���������꣬�����ӽ��������ʵ��λ�á�
		FVector Start = CrosshariWorldPosition;

		// �������������ű����������
		FVector End = Start + CrosshariWorldDirection * 80000.f;

		
		// ���ߴ�start��ʼ������ WorldDirection ��������80000��λ������Ƿ���ײ���κ����塣������ײ���Ŀ����Ϣ����TraceHitResult��
		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);

		// �����ײ,���û�з�����ײ���ǾͰ��յ�λ����Ϊ��ײ�㣨һ������գ�
		if (!TraceHitResult.bBlockingHit)
		{
			// ��ײ�������
			TraceHitResult.ImpactPoint = End;
			HitTarget = End;
		}
		else
		{
			HitTarget = TraceHitResult.ImpactPoint;
			// ��������뾶
			// ��� bBlockingHit Ϊ true������������ײ��������� DrawDebugSphere ��������ײ�����һ����ɫ�ĵ������塣
			DrawDebugSphere(
				GetWorld(),
				TraceHitResult.ImpactPoint,
				12.f,	// ����İ뾶
				12,	// �����ϸ������������ķֶ��������ڿ��������ƽ���ȣ���
				FColor::Red	// �������ɫ����ɫ��
			);
		}
	}



}

// �ಥRPC���ã��ڷ������˵��öಥrpc
void UCombatComponent::MulticastFire_Implementation()
{
	if (EquipedWeapon == nullptr) return;
	if (Character )
	{
		Character->PlayFireMontage(bAiming);
		
		EquipedWeapon->Fire(HitTarget);
	}
}

// Զ��RPC����
void UCombatComponent::ServerFire_Implementation()
{
	MulticastFire();
}
	


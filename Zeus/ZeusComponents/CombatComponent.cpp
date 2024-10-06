// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Zeus/Weapon/Weapon.h"
#include "Zeus/Character/ZeusCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	// ����Ҫÿ֡�����£���Ϊ���ǰ������������ϵĶ���
	PrimaryComponentTick.bCanEverTick = false;


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

	// ...
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Zeus/Weapon/Weapon.h"
#include "Zeus/Character/ZeusCharacter.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	// ����Ҫÿ֡�����£���Ϊ���ǰ������������ϵĶ���
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

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
	// ���ؼ��������ı�
	EquipedWeapon->ShowPickupWidget(false);
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


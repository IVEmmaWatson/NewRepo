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
	// 不需要每帧都更新，因为这是把武器拿在手上的动作
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

// 这个函数只会在服务器上执行
void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	// 检查角色指针和武器指针是否为空
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	 
	EquipedWeapon = WeaponToEquip;

	EquipedWeapon->SetWeaponState(EWeaponState::EWS_Equiped);
	// 从角色骨骼获取手部插槽
	// AttachActor：这个方法将一个 Actor 附加到插槽上。
	// 第一个参数是要附加的 Actor（在你的例子中是 EquipedWeapon），第二个参数是要附加到的骨骼网格。
	const USkeletalMeshSocket* HandSocket=Character->GetMesh()->GetSocketByName(FName("righthandsocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(EquipedWeapon,Character->GetMesh());
	}
	// 设置这个武器实例的拥有者
	EquipedWeapon->SetOwner(Character);
	
	// 当这个属性为 false 时，角色不会根据移动方向来调整自身的朝向。
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	// 设置角色跟着控制器旋转，当这个属性为 true时，角色会根据控制器的旋转来调整自身的朝向。
	Character->bUseControllerRotationYaw = true;
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...设置角色移动速度
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed= BaseWalkSpeed;
	}

	
}

// 通过函数设置是否在瞄准，判断是否是服务器角色，如果是客户端角色则调用rpc函数
// 因为客户端是复制角色没有与服务器的实际角色进行通信只能在本地看到自己的姿势
// 所以当客户端按下瞄准后需要通知服务器角色也进入瞄准
void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming; // 这句可以使客户端按下就可以看到瞄准姿势，不然还要等下面的rpc函数调用后才能看到姿势
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

// 当EquipedWeapon变量发生变化时调用此函数改变玩家的旋转朝向
// 需要当玩家拿着武器时，角色移动方向随鼠标控制，就是拿着武器的一面始终朝前，即相当于第一人称模式
void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquipedWeapon && Character)
	{
		// 当这个属性为 false 时，角色不会根据移动方向来调整自身的朝向。
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		// 设置角色跟着控制器旋转，当这个属性为 true时，角色会根据控制器的旋转来调整自身的朝向。
		Character->bUseControllerRotationYaw = true;
	}
}

// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


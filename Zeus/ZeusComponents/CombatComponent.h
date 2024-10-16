// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Zeus/HUD/ZeusHUD.h"
#include "Zeus/Weapon/WeaponTypes.h"
#include "Zeus/ZeusTypes/CombatState.h"
#include "CombatComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZEUS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();

	// 友元函数friend
	friend class AZeusCharacter;

	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// 装备武器，需要一个武器的指针 
	void EquipWeapon(class AWeapon* WeaponToEquip);

	void Reload();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);

	UFUNCTION(Server,Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void FireButtonPressed(bool bPressed);

	void Fire();

	UFUNCTION(Server,Reliable)
	void ServerFire(const FVector_NetQuantize TraceHitTarget);

	UFUNCTION(NetMulticast,Reliable)
	void MulticastFire(const FVector_NetQuantize TraceHitTarget);

	void TraceUnderCorsshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

	UFUNCTION(Server,Reliable)
	void ServerReload();

	void HandleReload();
	int32 AmountToReload();
	
	UFUNCTION(BlueprintCallable)
	void FinishReloading();
private:
	// 声明角色和武器类的指针，还没有实例化
	class AZeusCharacter* Character;
	class AZeusPlayerController* Controller;
	class AZeusHUD* HUD;

	// 将EquipedWeapon变量复制到网络中通信
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquipedWeapon;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	bool bFireButtonPressed;

	FVector HitTarget;
	// 准星设置的结构体
	FHUDPackage HUDPackage;

	// 准星速度因子
	float CrosshairVelocityFactor;
	// 准星空中因子
	float CrosshairInAirFactor;
	// 准星瞄准因子
	float CrosshairAimFactor;
	// 准星射击因子
	float CrosshairShootingFactor;

	// 瞄准的FOV视野变化

	// 保存默认视野值。用于在角色未瞄准或缩放时的正常视野。
	float DefaultFOV;

	// 定义缩放时的视野值。这个值表示当角色瞄准或使用武器瞄准镜时，视野将缩小到多少度。
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomedFOV = 30.f;

	
	float CurrentFOV;

	// 定义缩放时视野插值的速度。这个值表示视野从默认视野过渡到缩放视野的速度。
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);

	// 自动开火
	FTimerHandle FireTimer;

	
	bool bCanFire=true;

	void StartFireTimer();
	void	FireTimerFinished();

	bool CanFire();

	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	// 存储武器类型的字典
	TMap<EWeaponType, int32> CarriedAmmoMap;

	// 弹夹数量
	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30;
	void InitializeCarriedAmmo();

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState=ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	void UpdateAmmoValues();
public:

};

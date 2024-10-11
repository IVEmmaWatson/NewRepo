// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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

	UFUNCTION(Server,Reliable)
	void ServerFire(const FVector_NetQuantize TraceHitTarget);

	UFUNCTION(NetMulticast,Reliable)
	void MulticastFire(const FVector_NetQuantize TraceHitTarget);

	void TraceUnderCorsshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);
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
public:

};

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

	// ��Ԫ����friend
	friend class AZeusCharacter;

	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// װ����������Ҫһ��������ָ�� 
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
	// ������ɫ���������ָ�룬��û��ʵ����
	class AZeusCharacter* Character;
	class AZeusPlayerController* Controller;
	class AZeusHUD* HUD;

	// ��EquipedWeapon�������Ƶ�������ͨ��
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
	// ׼�����õĽṹ��
	FHUDPackage HUDPackage;

	// ׼���ٶ�����
	float CrosshairVelocityFactor;
	// ׼�ǿ�������
	float CrosshairInAirFactor;
	// ׼����׼����
	float CrosshairAimFactor;
	// ׼���������
	float CrosshairShootingFactor;

	// ��׼��FOV��Ұ�仯

	// ����Ĭ����Ұֵ�������ڽ�ɫδ��׼������ʱ��������Ұ��
	float DefaultFOV;

	// ��������ʱ����Ұֵ�����ֵ��ʾ����ɫ��׼��ʹ��������׼��ʱ����Ұ����С�����ٶȡ�
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomedFOV = 30.f;

	
	float CurrentFOV;

	// ��������ʱ��Ұ��ֵ���ٶȡ����ֵ��ʾ��Ұ��Ĭ����Ұ���ɵ�������Ұ���ٶȡ�
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);

	// �Զ�����
	FTimerHandle FireTimer;

	
	bool bCanFire=true;

	void StartFireTimer();
	void	FireTimerFinished();

	bool CanFire();

	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	// �洢�������͵��ֵ�
	TMap<EWeaponType, int32> CarriedAmmoMap;

	// ��������
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

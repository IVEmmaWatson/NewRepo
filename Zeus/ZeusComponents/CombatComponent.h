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

	// ��Ԫ����friend
	friend class AZeusCharacter;

	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// װ����������Ҫһ��������ָ�� 
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
public:

};

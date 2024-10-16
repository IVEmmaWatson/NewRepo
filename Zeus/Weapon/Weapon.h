// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimationAsset.h"
#include "WeaponTypes.h"
#include "Weapon.generated.h"



UENUM()
// enum class ��C++11�����ǿ����ö��.EWeaponStateö�����ơ�: uint8 ָ����ö�ٵĵײ�����Ϊ uint8���޷���8λ������
enum class EWeaponState :uint8
{
	// EWS_Initial ��ö�ٵĵ�һ����Ա����ʾ��ʼ״̬��
	// UMETA(DisplayName = "Initial State") ��һ��Ԫ���ݺ꣬����ָ���ڱ༭������ʾ�����ơ�
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equiped UMETA(DisplayName = "Equiped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")

};

UCLASS()
class ZEUS_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	

	AWeapon();
	virtual void Tick(float DeltaTime) override;
	// ����Ҫ�������縴�Ƶ����Էŵ����������
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void ShowPickupWidget(bool bShowWidget);
	void SetWeaponState(EWeaponState State);
	virtual void Fire(const FVector& HitTarget);
	void Dropped();
	virtual	void OnRep_Owner() override;
	void SetHUDAmmo();
	void AddAmmo(int32 AmmoToAdd);

	// UTexture2D������Unreal Engine�е�һ���࣬����2D������Щ����ͨ��������Ϸ�е�ͼ����HUDԪ�ء���ɫ����ȡ�
	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	class UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	class UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	class UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	class UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	class UTexture2D* CrosshairsBottom;

	// ��ͬ��������Ұ��������������ͼ������
	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;


	UPROPERTY(EditAnywhere, Category = "Combat")
	// ��������
	float FireDelay = .15f;

	// ������Щ�������Զ�����
	UPROPERTY(EditAnywhere, Category = "Combat")
	bool bAutomatic = true;

	UPROPERTY(EditAnywhere)
	class USoundCue* EquipSound;
protected:

	virtual void BeginPlay() override;

	// �ص�����
	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OhterActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	// UFUNCTION()�����д�˲���ʹ�ûص�
	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OhterActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
		);

private:
	// USkeletalMeshComponent�����ڹ����������������һ��Ĺ�������֧�ֶ���������ģ�⡣
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	// ��ײ�����������ڴ����͹���һ�����ε���ײ����
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* AreaSphere;

	// ö�����������ͼ��������,�����������������ĸ��ƺ����󶨣���������������ı�ʱ������ĺ����ͻ����
	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();
	// ��ͼ�ı��ؼ���������������
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidget;
	
	// ����һ����������
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UAnimationAsset* FireAnimation;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasing> CasingClass;

	// �ӵ�����
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo)
	int32 Ammo;

	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	UFUNCTION()
	void OnRep_Ammo();

	UFUNCTION()
	void SpendRound();

	UPROPERTY()
	class AZeusCharacter* ZeusOwnerCharacter;
	UPROPERTY()
	class AZeusPlayerController* ZeusOwnerPlayerController;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;
public:
	// ��������״̬
	 
	// ������ײ�����¼�
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	bool IsEmpty();

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapactiy() const { return MagCapacity; }
};

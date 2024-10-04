// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	void ShowPickupWidget(bool bShowWidget);
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

	// ö�����������ͼ��������
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	// ��ͼ�ı��ؼ���������������
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidget;

public:
	// ��������״̬
	FORCEINLINE void SetWeaponState(EWeaponState State) { WeaponState = State; }
};

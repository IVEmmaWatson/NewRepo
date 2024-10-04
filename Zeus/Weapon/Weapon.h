// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"


UENUM()
// enum class 是C++11引入的强类型枚举.EWeaponState枚举名称。: uint8 指定了枚举的底层类型为 uint8（无符号8位整数）
enum class EWeaponState :uint8
{
	// EWS_Initial 是枚举的第一个成员，表示初始状态。
	// UMETA(DisplayName = "Initial State") 是一个元数据宏，用于指定在编辑器中显示的名称。
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

	// 回调函数
	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OhterActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	// UFUNCTION()这个宏写了才能使用回调
	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OhterActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
		);

private:
	// USkeletalMeshComponent：用于管理骨骼和网格结合在一起的骨骼网格，支持动画和物理模拟。
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	// 碰撞检测组件，用于创建和管理一个球形的碰撞区域
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* AreaSphere;

	// 枚举类变量与蓝图关联起来
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	// 蓝图文本控件与武器关联起来
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidget;

public:
	// 设置武器状态
	FORCEINLINE void SetWeaponState(EWeaponState State) { WeaponState = State; }
};

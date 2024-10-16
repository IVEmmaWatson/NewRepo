// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimationAsset.h"
#include "WeaponTypes.h"
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
	// 将需要进行网络复制的属性放到这个数组里
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void ShowPickupWidget(bool bShowWidget);
	void SetWeaponState(EWeaponState State);
	virtual void Fire(const FVector& HitTarget);
	void Dropped();
	virtual	void OnRep_Owner() override;
	void SetHUDAmmo();
	void AddAmmo(int32 AmmoToAdd);

	// UTexture2D：这是Unreal Engine中的一个类，代表2D纹理。这些纹理通常用于游戏中的图像，如HUD元素、角色纹理等。
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

	// 不同武器的视野能力，可以在蓝图中设置
	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;


	UPROPERTY(EditAnywhere, Category = "Combat")
	// 开火速率
	float FireDelay = .15f;

	// 设置哪些武器能自动开火
	UPROPERTY(EditAnywhere, Category = "Combat")
	bool bAutomatic = true;

	UPROPERTY(EditAnywhere)
	class USoundCue* EquipSound;
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

	// 枚举类变量与蓝图关联起来,并将这个属性与下面的复制函数绑定，当这个变量发生改变时，下面的函数就会调用
	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();
	// 蓝图文本控件与武器关联起来
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidget;
	
	// 声明一个动画序列
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UAnimationAsset* FireAnimation;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasing> CasingClass;

	// 子弹数量
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
	// 设置武器状态
	 
	// 设置碰撞区域事件
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	bool IsEmpty();

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapactiy() const { return MagCapacity; }
};

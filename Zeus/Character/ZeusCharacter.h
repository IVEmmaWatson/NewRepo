// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ZeusCharacter.generated.h"

UCLASS()
class ZEUS_API AZeusCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AZeusCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();

public:	
	// Called every frame
	// 这个函数每一帧都会被调用，用于更新对象的状态。DeltaTime 参数表示自上一帧以来经过的时间（以秒为单位），
	// 这对于实现平滑的动画和物理效果非常重要。
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	// 这个函数用于绑定玩家输入到特定的功能上。PlayerInputComponent 是一个输入组件，
	// 它允许你将各种输入事件（如按键、鼠标移动等）绑定到相应的处理函数上。
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 这个函数的主要作用是将需要在网络上进行复制的属性添加到 OutLifetimeProps 数组中
	// TArray<FLifetimeProperty>模板类数组定义，即FLifetimeProperty类型得到数组
	// FLifetimeProperty 是一个结构体，用于描述需要复制的属性。
	// OutLifetimeProps是OutLifetimeProps 是一个引用，指向一个 TArray<FLifetimeProperty> 类型的数组
	// const 这个关键字表示函数不会修改类的成员变量。s
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// 用于在所有组件初始化完成后进行额外的初始化操作，在beginpaly之前被调用
	virtual void PostInitializeComponents() override;

private:
	// 定义变量的属性，如控制变量在编辑器中的可见性、可编辑性，以及它们在蓝图中的访问权限
	// 设置VisibleAnywhere：在编辑器中可见，但不可编辑。Category：将变量归类到特定类别中，方便在编辑器中组织和查找。
	UPROPERTY(VisibleAnywhere, Category = Camera)
	// 配置摄像机和角色直接的支架
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	// 配置跟随角色移动的摄像机
	class UCameraComponent* FollowCamera;
	
	// 声明一个用户控件组件的指针，允许在编辑器中编辑，允许在蓝图中读取该属性，允许在私有成员变量上使用UPROPERTY宏，使其在编辑器和蓝图中可见。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess="true"))
	class UWidgetComponent* OverHeadWidget;

	// 声明一个指向 AWeapon 类对象的指针 OverlappingWeapon，并使其在网络上进行复制，以确保客户端和服务器之间的数据一致性。
	// ReplicatedUsing = OnRep_OverlappingWeapon: 这个属性表示，当 OverlappingWeapon 在服务器上发生变化时，
	// 客户端会调用 OnRep_OverlappingWeapon 函数。
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	// ReplicatedUsing 属性的工作机制
	// 当你使用 UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon) 标记一个变量时
	// 在调用 OnRep_OverlappingWeapon 函数时，Unreal Engine 会将 OverlappingWeapon 变量变化前的值（即之前的武器指针）
	// 作为参数传递给 OnRep_OverlappingWeapon 函数的 LastWeapon 参数。
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);


	// 战斗组件类指针，在角色蓝图里关联起来
	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;

	// 远程过程调用RPC，Server：这个修饰符表示函数将在服务器上执行。客户端调用这个函数时，Unreal Engine 会将调用请求发送到服务器，服务器接收到请求后执行该函数。
	// Reliable：这个修饰符表示函数调用是可靠的，确保调用不会丢失。即使网络状况不佳，Unreal Engine 也会重试发送，直到服务器成功接收到并执行。
	// 服务器需要有这个函数的执行代码。具体来说，当你在客户端调用一个带有 Server 标记的函数时，Unreal Engine 会将这个调用请求发送到服务器，并在服务器上执行相应的实现代码。
	UFUNCTION(Server,Reliable)
	void ServerEquipButtonPressed();



public:
	// 网络复制的变量，只有在服务器上的属性真的发生变化时才会与客户端通信，告诉客户端该属性变化了
	// 这里只是通知了客户端属性改变了，并没有通知服务器，Replicated网络复制的工作方式只存在从服务器通知客户端
	// FORCEINLINE void SetOverlappingWeapon(AWeapon* Weapon) { OverlappingWeapon = Weapon; };
	void SetOverlappingWeapon(AWeapon* Weapon);
	// 表示是否处于状态武器状态
	bool IsWeaponEquipped();
	// 表示是否处于武器瞄准状态
	bool IsAiming();
};

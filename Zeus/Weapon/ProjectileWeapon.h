// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Projectile.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class ZEUS_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()
public:
	virtual void Fire(const FVector& HitTarget) override;
private:
	//? 声明一个动态类变量，TSubclassOf<class AProjectile> 用于存储类的类型信息。这种方式允许你在运行时动态地指定一个类，而不需要在编译时就确定具体的类。
	//? 例如，ProjectileClass 可以是 AProjectile 或者 AProjectile 的任何子类。
	//? TSubclassOf<AProjectile> 存储的是类型信息。这意味着 ProjectileClass 不存储具体的类实例或对象，而是存储类的类型本身。
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileClass;
};

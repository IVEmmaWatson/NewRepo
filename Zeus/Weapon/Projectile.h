// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class ZEUS_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OhterActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:	
	// Called every frame
	
private:

	// 声明一个长方体矩形盒型的碰撞区域组件
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	// 粒子系统类，主要设置和定义粒子的属性
	UPROPERTY(EditAnywhere)
	class UParticleSystem* Tracer;

	// 粒子组件类，负责把定义好的粒子系统加载到游戏世界中，并控制其行为
	class UParticleSystemComponent* TracerComponent;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles;

	// 音效类
	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;
};

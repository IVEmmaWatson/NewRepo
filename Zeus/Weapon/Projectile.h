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

	// ����һ����������κ��͵���ײ�������
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	// ����ϵͳ�࣬��Ҫ���úͶ������ӵ�����
	UPROPERTY(EditAnywhere)
	class UParticleSystem* Tracer;

	// ��������࣬����Ѷ���õ�����ϵͳ���ص���Ϸ�����У�����������Ϊ
	class UParticleSystemComponent* TracerComponent;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles;

	// ��Ч��
	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;
};

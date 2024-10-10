// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

UCLASS()
class ZEUS_API ACasing : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACasing();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OhterActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	
private:
	UPROPERTY(VisibleAnywhere, Category = "wanggeti")
	UStaticMeshComponent* CasingMesh;

	
	UPROPERTY(EditAnywhere)
	float ShellEjectionImpulse;

	// …˘“ÙÃÿ–ß
	UPROPERTY(EditAnywhere)
	class USoundCue* ShellSound;
};


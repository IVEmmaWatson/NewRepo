// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ZeusPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ZEUS_API AZeusPlayerController : public APlayerController
{
	GENERATED_BODY()
	

public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	virtual void OnPossess(APawn* InPawn) override;
protected:
	virtual void BeginPlay() override;
private:
	class AZeusHUD* ZeusHUD;
};

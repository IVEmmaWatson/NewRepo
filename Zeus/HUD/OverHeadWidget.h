// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverHeadWidget.generated.h"

/**
 * 
 */
UCLASS()
class ZEUS_API UOverHeadWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 创建一个用户控件（User Widget）蓝图时，通常需要新建一个C++类并在其中使用BindWidget来绑定蓝图中的控件。
	UPROPERTY(meta = (BindWidget));
	class UTextBlock* DisplayText;

	void SetDisplayText(FString TextToDisplay);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(APawn* InPawn);

protected:
	virtual void NativeDestruct()override;
};

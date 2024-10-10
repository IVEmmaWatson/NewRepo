// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ZeusHUD.generated.h"


//t USTRUCT: 这是一个宏，用于告诉Unreal Engine这个结构体需要进行反射处理，使其能够与引擎的其它系统（如蓝图）进行交互。
//t BlueprintType: 这是一个修饰符，表示这个结构体可以在蓝图中使用。加上这个修饰符后，可以在蓝图中定义和操作这个结构体。
USTRUCT(BlueprintType)
struct FHUDPackage
{
	//? 这个宏用于生成结构体的默认构造函数、复制构造函数、赋值操作符等必要的代码。这是Unreal Engine反射系统的一部分，它可以确保这个结构体能被引擎正确识别和处理。
	GENERATED_BODY()

public:
	class UTexture2D* CorsshairsCenter;
	UTexture2D* CorsshairsLeft;
	UTexture2D* CorsshairsRigth;
	UTexture2D* CorsshairsTop;
	UTexture2D* CorsshairsBottom;
};
// 这个结构体定义了一个HUD包，包含五个不同方向的准星纹理类。

/**
 * 
 */
UCLASS()
class ZEUS_API AZeusHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void DrawHUD() override;

private:
	// 声明一个结构体变量
	FHUDPackage HUDPackage;
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter);
public:
	// 内联函数，赋值给这个结构体
	FORCEINLINE void SetHUDPackeage(const FHUDPackage& Package) { HUDPackage = Package; }
};

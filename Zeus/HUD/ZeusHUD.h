// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ZeusHUD.generated.h"


//t USTRUCT: ����һ���꣬���ڸ���Unreal Engine����ṹ����Ҫ���з��䴦��ʹ���ܹ������������ϵͳ������ͼ�����н�����
//t BlueprintType: ����һ�����η�����ʾ����ṹ���������ͼ��ʹ�á�����������η��󣬿�������ͼ�ж���Ͳ�������ṹ�塣
USTRUCT(BlueprintType)
struct FHUDPackage
{
	//? ������������ɽṹ���Ĭ�Ϲ��캯�������ƹ��캯������ֵ�������ȱ�Ҫ�Ĵ��롣����Unreal Engine����ϵͳ��һ���֣�������ȷ������ṹ���ܱ�������ȷʶ��ʹ���
	GENERATED_BODY()

public:
	class UTexture2D* CorsshairsCenter;
	UTexture2D* CorsshairsLeft;
	UTexture2D* CorsshairsRigth;
	UTexture2D* CorsshairsTop;
	UTexture2D* CorsshairsBottom;
};
// ����ṹ�嶨����һ��HUD�������������ͬ�����׼�������ࡣ

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
	// ����һ���ṹ�����
	FHUDPackage HUDPackage;
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter);
public:
	// ������������ֵ������ṹ��
	FORCEINLINE void SetHUDPackeage(const FHUDPackage& Package) { HUDPackage = Package; }
};

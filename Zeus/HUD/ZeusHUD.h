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
	class UTexture2D* CrosshairsCenter;
	UTexture2D* CrosshairsLeft;
	UTexture2D* CrosshairsRight;
	UTexture2D* CrosshairsTop;
	UTexture2D* CrosshairsBottom;
	// ʮ��׼�ǵ���ɫ
	FLinearColor CrosshairColor;

	// ʮ��׼�ǵĻ��Ʊ߽磬����Ļ���ĵ�ľ���
	float CrosshairSpread;
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
	
	// ������һ����Ϊ CharacterOverlayClass �����ԣ�������Կ����ڱ༭�����κεط����б༭�����ұ������� "Player Stats" �����¡������������� TSubclassOf<UUserWidget>��
	// ��������Կ��Դ洢����һ�� UUserWidget �����ࡣ
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;

	void AddCharacterOverlay();

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	UPROPERTY(EditAnywhere, Category = "Announcements")
	TSubclassOf<class UUserWidget> AnnouncementClass;

	UPROPERTY()
	class UAnnouncement* Announcement;

	void AddAnnouncement();
protected:
	virtual void BeginPlay() override;
	
private:
	// ����һ���ṹ�����
	FHUDPackage HUDPackage;
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter,FVector2D Spread,FLinearColor CrosshairColor);

	// ׼�������ɢֵ
	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;
public:
	// ������������ֵ������ṹ��
	FORCEINLINE void SetHUDPackeage(const FHUDPackage& Package) { HUDPackage = Package; }
};

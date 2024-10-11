// Fill out your copyright notice in the Description page of Project Settings.


#include "ZeusHUD.h"

// DrawHUD ������������Ļ��ƻ������úͳ������ƣ����������ͨ����д DrawHUD ����Զ����HUDԪ�ء�
// ��������������Ļ���ÿ֡������������ػ�
void AZeusHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportsSize;

	if (GEngine)
	{
		// ��ȡ��Ļ�ߴ�浽ViewportsSize������
		GEngine->GameViewport->GetViewportSize(ViewportsSize);
		const FVector2D ViewportCenter(ViewportsSize.X / 2, ViewportsSize.Y / 2);

		// HUDPackage.CrosshairSpread�Ǹ��ݽ�ɫ�ٶ���������
		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		// ������������
		if (HUDPackage.CrosshairsCenter)
		{
			// ׼�ǵ㲻�䣬����ɢ��
			FVector2D Spread(0.f, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter,Spread);
		}
		if (HUDPackage.CrosshairsLeft)
		{
			//t x�������ת
			FVector2D Spread(-SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter, Spread);
		}
		if (HUDPackage.CrosshairsRight)
		{
			//t x�����ұ�ת
			FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter, Spread);
		}
		if (HUDPackage.CrosshairsTop)
		{
			// Y������
			FVector2D Spread(0.f ,- SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter, Spread);
		}
		if (HUDPackage.CrosshairsBottom)
		{
			FVector2D Spread(0.f ,SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCenter, Spread);
		}
	}
}

void AZeusHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter,FVector2D Spread)
{
	// ��ȡ����Ŀ�Ⱥ͸߶�
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	//t ��ȡҪ������������Ļ�ϵ�λ�ã����������Ǵ��������Ͻǿ�ʼ����
	//t ���ֱ������Ļ������Ϊ���Ƶ㣬����������ϽǾ�����Ļ���ģ�������Ҫ��ȥһ�������߶ȺͿ�ȣ������������Ҳ����Ļ����
	// ��������Ͻ������������˵�ǲ���Ҳ��һ����Ļԭ�㣬Ҳ����0��0���꣬�����½Ǿ���1��1����
	// ����׼����ɢ��ֵ�������ɫ�ٶ�С�������spreadֵ��С�������������ƾ�С���У����spread��������ͻ��ƵĴ�
	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f)+Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f)+Spread.Y
	);

	DrawTexture(
		Texture,	// ���Ƶ�����
		TextureDrawPoint.X,	// �������X
		TextureDrawPoint.Y,		// �������Y
		TextureWidth, TextureHeight,		// ����߶ȿ��
		0.f, 0.f,	// ��ʼ�����ʾ����������Ͻǿ�ʼ������0.f ��Ӧ�����U�ᣨˮƽ�ᣩ��㣬0.f ��Ӧ�����V�ᣨ��ֱ�ᣩ��㡣����ζ�Ŵ���������Ͻǿ�ʼ���ơ�
		1.f, 1.f,	// ��ֹ���� (End U, End V)����ʾ����Ĳ�����ֹλ�á�1.f ��Ӧ�����U���յ㣨�ұ�Ե����1.f ��Ӧ�����V���յ㣨�±�Ե��������ζ�Ż����������������Ͻǵ����½�ȫ����ʾ����
		FColor::White
	);


}

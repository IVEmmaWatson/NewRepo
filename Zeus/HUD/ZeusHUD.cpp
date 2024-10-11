// Fill out your copyright notice in the Description page of Project Settings.


#include "ZeusHUD.h"

// DrawHUD 函数负责基础的绘制环境设置和初步绘制，而子类可以通过重写 DrawHUD 添加自定义的HUD元素。
// 在这个函数中做的绘制每帧都会清除掉再重绘
void AZeusHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportsSize;

	if (GEngine)
	{
		// 获取屏幕尺寸存到ViewportsSize变量里
		GEngine->GameViewport->GetViewportSize(ViewportsSize);
		const FVector2D ViewportCenter(ViewportsSize.X / 2, ViewportsSize.Y / 2);

		// HUDPackage.CrosshairSpread是根据角色速度来调整的
		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		// 绘制所有纹理
		if (HUDPackage.CrosshairsCenter)
		{
			// 准星点不变，四面散开
			FVector2D Spread(0.f, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter,Spread);
		}
		if (HUDPackage.CrosshairsLeft)
		{
			//t x轴往左边转
			FVector2D Spread(-SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter, Spread);
		}
		if (HUDPackage.CrosshairsRight)
		{
			//t x轴往右边转
			FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter, Spread);
		}
		if (HUDPackage.CrosshairsTop)
		{
			// Y轴往上
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
	// 获取纹理的宽度和高度
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	//t 获取要绘制纹理在屏幕上的位置，绘制纹理都是从纹理左上角开始绘制
	//t 如果直接用屏幕中心作为绘制点，那纹理的左上角就在屏幕中心，所以需要减去一半的纹理高度和宽度，让纹理的中心也在屏幕中心
	// 纹理的左上角相对于纹理来说是不是也是一个屏幕原点，也就是0，0坐标，那右下角就是1，1坐标
	// 加上准星扩散的值，如果角色速度小，那这个spread值就小，那这个纹理绘制就小集中，如果spread大那这个就绘制的大
	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f)+Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f)+Spread.Y
	);

	DrawTexture(
		Texture,	// 绘制的纹理
		TextureDrawPoint.X,	// 绘制起点X
		TextureDrawPoint.Y,		// 绘制起点Y
		TextureWidth, TextureHeight,		// 纹理高度宽度
		0.f, 0.f,	// 起始坐标表示从纹理的左上角开始采样。0.f 对应纹理的U轴（水平轴）起点，0.f 对应纹理的V轴（垂直轴）起点。这意味着从纹理的左上角开始绘制。
		1.f, 1.f,	// 终止坐标 (End U, End V)：表示纹理的采样终止位置。1.f 对应纹理的U轴终点（右边缘），1.f 对应纹理的V轴终点（下边缘）。这意味着绘制整个纹理，从左上角到右下角全部显示出来
		FColor::White
	);


}

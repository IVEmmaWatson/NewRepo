// Fill out your copyright notice in the Description page of Project Settings.


#include "OverHeadWidget.h"
#include "Components/TextBlock.h"

void UOverHeadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverHeadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	// GetLocalRole返回本地计算机对此参与者的控制程度
	// getremoterole返回远程计算机对此参与者的控制程度
	ENetRole LocalRole = InPawn->GetLocalRole();
	FString Role;
	switch (LocalRole)
	{
	case ROLE_None:
		Role = FString("None");
		break;
		// 模拟代理，一般是本地计算机上其他玩家，数据都是服务器复制过来的
	case ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy");
		break;
		// 参与者的本地代理，服务器生成的发送的副本
	case ROLE_AutonomousProxy:
		Role = FString("Autonomous Proxy");
		break;
		// 一般是服务器上的，服务器生成的
	case ROLE_Authority:
		Role = FString("Authority");
		break;
	}
	FString LocalRoleString = FString::Printf(TEXT("LocalRole is: %s "), *Role);
	SetDisplayText(LocalRoleString);
}

void UOverHeadWidget::NativeDestruct()
{
	Super::NativeDestruct();
	RemoveFromParent();
}


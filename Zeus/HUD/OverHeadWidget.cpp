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
	// GetLocalRole���ر��ؼ�����Դ˲����ߵĿ��Ƴ̶�
	// getremoterole����Զ�̼�����Դ˲����ߵĿ��Ƴ̶�
	ENetRole LocalRole = InPawn->GetLocalRole();
	FString Role;
	switch (LocalRole)
	{
	case ROLE_None:
		Role = FString("None");
		break;
		// ģ�����һ���Ǳ��ؼ������������ң����ݶ��Ƿ��������ƹ�����
	case ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy");
		break;
		// �����ߵı��ش������������ɵķ��͵ĸ���
	case ROLE_AutonomousProxy:
		Role = FString("Autonomous Proxy");
		break;
		// һ���Ƿ������ϵģ����������ɵ�
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


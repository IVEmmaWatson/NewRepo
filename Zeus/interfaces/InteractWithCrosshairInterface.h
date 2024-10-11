// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractWithCrosshairInterface.generated.h"

// This class does not need to be modified.
//t MinimalAPI ��һ���÷��������Ż�ģ���API�ı�¶�������֮��MinimalAPI ����Ҫ�����Ǽ��ٲ���Ҫ�ķ��ű�¶���Ӷ�ʹ�ñ�������ӹ��̸���Ч��
//t �����̳���UInterface���������ᱻʵ������������Ҫ�����Ǹ���Unreal Engine����һ���ӿڣ���Ϊ����ϵͳ�ṩ֧�֡�
UINTERFACE(MinimalAPI)
class UInteractWithCrosshairInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * ����ʵ�ʵĽӿڣ������ཫʵ������ӿڡ�ZEUS_APIȷ������ģ��߽�֮����ȷ������
 * ZEUS_API�Ǹ�������ĺ궨�壬��ʾ�������Ա�������ʵ�ֵ���
 */
class ZEUS_API IInteractWithCrosshairInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractWithCrosshairInterface.generated.h"

// This class does not need to be modified.
//t MinimalAPI 是一种用法，用于优化模块间API的暴露。简而言之，MinimalAPI 的主要作用是减少不必要的符号暴露，从而使得编译和链接过程更高效。
//t 这个类继承自UInterface，它本身不会被实例化。它的主要作用是告诉Unreal Engine这是一个接口，并为反射系统提供支持。
UINTERFACE(MinimalAPI)
class UInteractWithCrosshairInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 声明实际的接口，其他类将实现这个接口。ZEUS_API确保类在模块边界之间正确公开。
 * ZEUS_API是个导出类的宏定义，表示这个类可以被其他类实现调用
 */
class ZEUS_API IInteractWithCrosshairInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
};

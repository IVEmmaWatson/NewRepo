// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// 宏替换：这行代码的作用是将 ECC_SkeletalMesh 定义为 ECollisionChannel::ECC_GameTraceChannel1，在代码中每次使用 ECC_SkeletalMesh 时，
// 编译器都会将其替换为 ECollisionChannel::ECC_GameTraceChannel1。
#define ECC_SkeletalMesh ECollisionChannel::ECC_GameTraceChannel1
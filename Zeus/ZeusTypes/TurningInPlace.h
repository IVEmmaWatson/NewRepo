#pragma once


UENUM(BlueprintType)
enum class ETurningInPlace: uint8
{
	ETIP_Left UMETA(DisplayName="Turning Left"),				//t 0
	ETIP_RIght UMETA(DisplayName = "Turning Right"),		//t 1
	ETIP_NotTurning UMETA(DisplayName = "Not Turning"),		//t 2

	ETIP_MAX UMETA(DisplayName = "DefaultMAX")			//t 3 ��ʾ���ֵ��3��Ҳ��ʾ��4��ö��ֵ
};
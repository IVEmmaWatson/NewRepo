// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include	"Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	// ������������ת����apawn����ͨ����������ת��������ȷ�� InstigatorPawn ��һ�� APawn ����
	// ָ�������ߣ�������� APawn ���ͣ���ָ�롣���ת��ʧ�ܣ��������߲��� APawn ���ͣ���InstigatorPawn ��Ϊ nullptr��
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	// ��ȡ����ǹ�ڵĲ��
	const USkeletalMeshSocket* MuzzleFlashSocket =GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));

	if (MuzzleFlashSocket)
	{
		// ��ȡ�����������������ռ�ı任��λ����ת���ţ�
		FTransform SocketTransform=MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		// ǹ�ܼ�˵��������е�Ŀ��
		// �������������ʾ����;��룬a-b��ʾbΪ��㵽aΪ�յ�ķ���;���
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		// ����������ת������ת����
		FRotator TargetRotation = ToTarget.Rotation();
		// ProjectileClassֻ�Ǵ洢��������Ϣ����������ʵ������󣬾͵�����ʾ�Ǹ��໹�Ǹ������һ���������ͣ������ж�
		if (ProjectileClass&&InstigatorPawn)
		{
			UWorld* World = GetWorld();
			//t FActorSpawnParameters ��һ���ṹ�壬���ڴ洢����Actorʱ��һЩ�������������ߡ��Ƿ�˲�����ɵȡ�
			FActorSpawnParameters SpawnParams;
			//t GetOwner() ��һ�����������ڻ�ȡ��ǰ�����Actor�������ߡ���ͨ����ָ�����������Actor���ϼ�����
			//t ��ǰ�ӵ�����������ǹ����ǹ�����������ǽ�ɫʵ�������Է��ص��ǽ�ɫʵ��
			SpawnParams.Owner = GetOwner();
			// ����������Ϊ��ǰ��ɫ
			SpawnParams.Instigator = InstigatorPawn;
			if (World)
			{
				// SpawnActor����һ����������Ķ���
				// ����1Ҫ���ɵĶ������ͣ�����2���ɵ����λ�ã�����3��������ʱ�ĳ��򣬲���4���ɲ����ṹ�壬�������������ߺ��������ɵĶ���
				World->SpawnActor<AProjectile>(
					ProjectileClass,
					SocketTransform.GetLocation(),
					TargetRotation,
					SpawnParams
					);

				
			}
		}
	}
}

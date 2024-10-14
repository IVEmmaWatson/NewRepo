// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// ApplyDamage����������Ҫ��ҿ������������Ȼ�ȡ�ӵ��������ߣ�getowner���ص���һ��actor�࣬������ת��������Acharacter��Ȼ���ȡ������
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		AController* OwnerController = OwnerCharacter->Controller;
		if (OwnerController)
		{
			UGameplayStatics::ApplyDamage(
				OtherActor,	// �ܵ��˺���Ŀ�� Actor��Ҳ������Ҫ��˭����˺���
				Damage,	// �˺�����ֵ�����ֵ��������ҪӦ�õ��˺�����ͨ����һ����������
				OwnerController,	// �˺�Դ�Ŀ�������ͨ������һ�� PlayerController �� AIController����ʾ��˭����������˺���
				this,	// ��������˺������壬���緢����ӵ���ը���������塣
				UDamageType::StaticClass());	// �˺����͵��ࡣ�������Ծ����˺������ࣨ��������˺��������˺��ȣ��Լ��������Ϊ
		}
	}

	


	// ��Ϊ����ĺ������ú�������ӵ�������������
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

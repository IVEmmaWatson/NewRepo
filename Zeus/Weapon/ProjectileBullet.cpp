// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// ApplyDamage第三个参数要玩家控制器，所以先获取子弹的所有者，getowner返回的是一个actor类，所以先转换成子类Acharacter，然后获取控制器
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		AController* OwnerController = OwnerCharacter->Controller;
		if (OwnerController)
		{
			UGameplayStatics::ApplyDamage(
				OtherActor,	// 受到伤害的目标 Actor。也就是你要对谁造成伤害。
				Damage,	// 伤害的数值。这个值代表你想要应用的伤害量，通常是一个浮点数。
				OwnerController,	// 伤害源的控制器。通常这是一个 PlayerController 或 AIController，表示是谁发起了这个伤害。
				this,	// 具体造成伤害的物体，例如发射的子弹、炸弹或者陷阱。
				UDamageType::StaticClass());	// 伤害类型的类。这个类可以决定伤害的种类（例如火焰伤害、冰冻伤害等）以及其具体行为
		}
	}

	


	// 因为父类的函数调用后会销毁子弹，所以最后调用
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

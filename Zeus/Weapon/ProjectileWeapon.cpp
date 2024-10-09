// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include	"Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	// 将所有者类型转换成apawn对象，通过这种类型转换，可以确保 InstigatorPawn 是一个 APawn 对象。
	// 指向所有者（如果它是 APawn 类型）的指针。如果转换失败（即所有者不是 APawn 类型），InstigatorPawn 将为 nullptr。
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	// 获取武器枪口的插槽
	const USkeletalMeshSocket* MuzzleFlashSocket =GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));

	if (MuzzleFlashSocket)
	{
		// 获取插槽相对于武器骨骼空间的变换（位置旋转缩放）
		FTransform SocketTransform=MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		// 枪管尖端到我们命中的目标
		// 向量坐标相减表示方向和距离，a-b表示b为起点到a为终点的方向和距离
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		// 将方向向量转换成旋转向量
		FRotator TargetRotation = ToTarget.Rotation();
		// ProjectileClass只是存储了类型信息，不包括类实例或对象，就单纯表示是父类还是父类的哪一个子类类型，可以判断
		if (ProjectileClass&&InstigatorPawn)
		{
			UWorld* World = GetWorld();
			//t FActorSpawnParameters 是一个结构体，用于存储生成Actor时的一些参数，如生成者、是否瞬间生成等。
			FActorSpawnParameters SpawnParams;
			//t GetOwner() 是一个方法，用于获取当前组件或Actor的所有者。这通常是指包含该组件或Actor的上级对象。
			//t 当前子弹的所有者是枪，而枪的所有者又是角色实例，所以返回的是角色实例
			SpawnParams.Owner = GetOwner();
			// 设置生成者为当前角色
			SpawnParams.Instigator = InstigatorPawn;
			if (World)
			{
				// SpawnActor生成一个在世界里的对象
				// 参数1要生成的对象类型，参数2生成弹丸的位置，参数3弹丸生成时的朝向，参数4生成参数结构体，包括生成所有者和引发生成的对象
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

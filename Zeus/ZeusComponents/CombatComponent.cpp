// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Zeus/Weapon/Weapon.h"
#include "Zeus/Character/ZeusCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Zeus/PlayerController/ZeusPlayerController.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"


#define TRACE_LENGTH 80000.f
// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	// ����Ҫÿ֡�����£���Ϊ���ǰ������������ϵĶ���
	PrimaryComponentTick.bCanEverTick = true;


	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;
	// ...
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquipedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
}

// �������ֻ���ڷ�������ִ��
void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	// ����ɫָ�������ָ���Ƿ�Ϊ��
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	if (EquipedWeapon)
	{
		EquipedWeapon->Dropped();
	 }
	EquipedWeapon = WeaponToEquip;

	EquipedWeapon->SetWeaponState(EWeaponState::EWS_Equiped);
	// �ӽ�ɫ������ȡ�ֲ����
	// AttachActor�����������һ�� Actor ���ӵ�����ϡ�
	// ��һ��������Ҫ���ӵ� Actor��������������� EquipedWeapon�����ڶ���������Ҫ���ӵ��Ĺ�������
	const USkeletalMeshSocket* HandSocket=Character->GetMesh()->GetSocketByName(FName("righthandsocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(EquipedWeapon,Character->GetMesh());
	}
	// �����������ʵ����ӵ����
	EquipedWeapon->SetOwner(Character);
	EquipedWeapon->SetHUDAmmo();
	// ���������Ϊ false ʱ����ɫ��������ƶ���������������ĳ���
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	// ���ý�ɫ���ſ�������ת�����������Ϊ trueʱ����ɫ����ݿ���������ת����������ĳ���
	Character->bUseControllerRotationYaw = true;
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...���ý�ɫ�ƶ��ٶ�
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed= BaseWalkSpeed;

		if (Character->GetFollowCamera())
		{
			// ��ȡ�����Ĭ����Ұֵ
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
	}

	
	
}

// ͨ�����������Ƿ�����׼���ж��Ƿ��Ƿ�������ɫ������ǿͻ��˽�ɫ�����rpc����
// ��Ϊ�ͻ����Ǹ��ƽ�ɫû�����������ʵ�ʽ�ɫ����ͨ��ֻ���ڱ��ؿ����Լ�������
// ���Ե��ͻ��˰�����׼����Ҫ֪ͨ��������ɫҲ������׼
void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming; // ������ʹ�ͻ��˰��¾Ϳ��Կ�����׼���ƣ���Ȼ��Ҫ�������rpc�������ú���ܿ�������
	if (!Character->HasAuthority())
	{
		ServerSetAiming(bIsAiming);
	}
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;


	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

// ��EquipedWeapon���������仯ʱ���ô˺����ı���ҵ���ת����
// ��Ҫ�������������ʱ����ɫ�ƶ������������ƣ���������������һ��ʼ�ճ�ǰ�����൱�ڵ�һ�˳�ģʽ
void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquipedWeapon && Character)
	{
		// �ͻ���Ҳִ��һ�������������״̬�͸��Ӳ�ۣ���ֹ���ӳٵ�����¿ͻ�����Ҫ�ȴ�ͬ�����ڼ����������ڴ����λ�á�
		EquipedWeapon->SetWeaponState(EWeaponState::EWS_Equiped);
		// �ӽ�ɫ������ȡ�ֲ����
		// AttachActor�����������һ�� Actor ���ӵ�����ϡ�
		// ��һ��������Ҫ���ӵ� Actor��������������� EquipedWeapon�����ڶ���������Ҫ���ӵ��Ĺ�������
		const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("righthandsocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(EquipedWeapon, Character->GetMesh());
		}

		// ���������Ϊ false ʱ����ɫ��������ƶ���������������ĳ���
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		// ���ý�ɫ���ſ�������ת�����������Ϊ trueʱ����ɫ����ݿ���������ת����������ĳ���
		Character->bUseControllerRotationYaw = true;
	}
}

// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	
	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCorsshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;

		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}
	
}

// ֻ����������º��׷������
void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed; 
	if (bFireButtonPressed&&EquipedWeapon)
	{
		// HitResult�����洢�������е�Ŀ����Ϣ
		
		Fire();
	}
}

void UCombatComponent::Fire()
{

	if (CanFire())
	{
		bCanFire = false;
		ServerFire(HitTarget);

		if (EquipedWeapon)
		{
			CrosshairShootingFactor = 0.52f;
		}
		StartFireTimer();
	}
	
}

// ���� FireDelay ����Ϊ1�룬��ô����Ұ��¿���ťʱ����ʱ���Ὺʼ��ʱ��
// 1������ FireTimerFinished ������FireTimerFinished ���������Ƿ���Ҫ�ٴο��𣬻���ֹͣ��ʱ���Է�ֹ��������


void UCombatComponent::StartFireTimer()
{
	if (EquipedWeapon == nullptr || Character == nullptr) return;
	Character->GetWorldTimerManager().SetTimer(
		// ����һ�� FTimerHandle ���͵ı��������ڴ洢�͹��������ʱ���ľ����
		FireTimer,
		// ����һ��ָ��ǰ�����ָ�룬�� UCombatComponent ʵ����
		this,
		// ����һ������ָ�룬ָ�� UCombatComponent ���е� FireTimerFinished ������
		&UCombatComponent::FireTimerFinished,
		// ���Ƕ�ʱ�����ӳ�ʱ��,��ʱ�����ȴ�ָ����ʱ�䣬Ȼ�󴥷� FireTimerFinished ������
		EquipedWeapon->FireDelay
	);
}

void UCombatComponent::FireTimerFinished()
{
	if (EquipedWeapon == nullptr) return;
	bCanFire = true;
	if (bFireButtonPressed && EquipedWeapon->bAutomatic)
	{
		Fire();
	}
}

bool UCombatComponent::CanFire()
{
	if (EquipedWeapon == nullptr) return false;
	// ����ӵ���Ϊ�գ���bCanFireΪ�ܿ�ǹ
	return !EquipedWeapon->IsEmpty() && bCanFire;
}

//t FHitResult��һ�����������߼�����е���ϸ��Ϣ����Ա����ImpactPoint�����е�λ�á�Normal�����е�ķ��ߡ�Actor�����е� Actor��Component�����е������
// TraceHitResult����Ҫ�ֶ�����ʹ���
// _TraceUnderCorsshairs����ͨ����������Ϸ�н������߼��
void UCombatComponent::TraceUnderCorsshairs(FHitResult& TraceHitResult)
{
	// ��ȡ��Ϸ��Ļ��ǰ�ĳߴ�
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// ��ȡ��Ϸ��Ļ�м������
	FVector2D CrosshariLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	FVector CrosshariWorldPosition;	// ���� WorldPosition ��ֵ�� (1000, 500, 200)�����ʾ��Ļ�ϵĵ� (100, 200) ��3D�����еľ���λ���� (1000, 500, 200)��
	FVector CrosshariWorldDirection;	// ���� WorldDirection ��ֵ�� (0.707, 0.707, 0)�����ʾ�����λ�õ� WorldPosition �ķ�����������������ǵ�λ��������ʾ��������Ǿ����λ�á�
	// ��һ����ά����ת������ά���꣬����1��ҿ�����ָ�룬����2Ҫת���Ķ�ά���꣬����3ת���������ռ�3D���꣬����4ת���������ռ䷽��
	bool bScreenToWorld=UGameplayStatics::DeprojectScreenToWorld(
		PlayerController,CrosshariLocation, CrosshariWorldPosition, CrosshariWorldDirection
	);

	
	// UGameplayStatics::GetPlayerController(this, 0);
	if (bScreenToWorld)
	{
		// ��ʼλ�ã�����Ļ�м�����άλ�����꣬һ��ʱ�����������
		// ��Ļ���ĵ��λ�ã�CrosshariWorldPosition��ͨ���ӽ����������λ�ã���Ϊ���Ǵ�������ӽ�ת�������ġ�
		// ���� DeprojectScreenToWorld ��������Ļ���ĵ�ͶӰ���������꣬�����ӽ��������ʵ��λ�á�
		FVector Start = CrosshariWorldPosition;

		// �����ʼ�ߵ���ɫ֮��ľ��룬��Ϊ��ʼ���Ǵ������������ģ�����׼�߲���ָ���ɫ���棬Ҳ�����������ɫ����һ�ξ���
		if (Character)
		{
			// Size() �������������ĳ��ȣ���������ŷ����þ��롣
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();

			// ����������㡣
			// ����ά�ռ��У�����������CrosshairWorldDirection���ṩ��һ����ȷ�ķ��򣬶�������DistanceToCharacter + 100.f��ֻ�Ǳ�ʾһ�����롣
			// Ҫ���ض��������ƶ�һ�����룬�ͱ��뽫���������������ˡ���ȷ������������ȷ�ķ����ƶ���
			Start += CrosshariWorldDirection * (DistanceToCharacter + 100.f);

			
		}



		// �������������ű����������
		FVector End = Start + CrosshariWorldDirection * 80000.f;

		

		// ���ߴ�start��ʼ������ WorldDirection ��������80000��λ������Ƿ���ײ���κ����塣������ײ���Ŀ����Ϣ����TraceHitResult��
		// ���û����ײ�����壬��TraceHitResult�����е�Ĭ��Ϊ000���ͻ�ɵ�����ԭ��
		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);

		// ��û����ײ������ʱ����ֹ�ӵ�������ԭ��
		if (!TraceHitResult.bBlockingHit)
		{
			// ��ײ�������
			TraceHitResult.ImpactPoint = End;
			// HitTarget = End;
		}

		// TraceHitResult.GetActor()������ײ����actor����ָ�룬Implements<UInteractWithCrosshairInterface>() ��һ��ģ�庯�������ڼ������� AActor �Ƿ�ʵ����ĳ���ӿڡ�
		// �����Ǽ����ײ�������actor���Ƿ�ʵ��������ӿ�
		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairInterface>())
		{
			HUDPackage.CrosshairColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairColor = FLinearColor::White;
		}
		// ����Ҫ���������ײ�������Ѿ������ײ�ˣ���ֻ�Ƿ������������û����Ч
		/*
		// ���������ײ,���û�з�����ײ���ǾͰ��յ�λ����Ϊ��ײ�㣨һ������գ�
		if (!TraceHitResult.bBlockingHit)
		{
			// ��ײ�������
			TraceHitResult.ImpactPoint = End;
			HitTarget = End;
		}

		else
		{
			HitTarget = TraceHitResult.ImpactPoint;
			
			// ��������뾶
			// ��� bBlockingHit Ϊ true������������ײ��������� DrawDebugSphere ��������ײ�����һ����ɫ�ĵ������塣
			DrawDebugSphere(
				GetWorld(),
				TraceHitResult.ImpactPoint,
				12.f,	// ����İ뾶
				12,	// �����ϸ������������ķֶ��������ڿ��������ƽ���ȣ���
				FColor::Red	// �������ɫ����ɫ��
			);
			
		}
		*/
	}



}


// �ಥRPC���ã��ڷ������˵��öಥrpc
//t FVector_NetQuantize �� FVector ��һ���Ż��汾��ͨ�������ķ�ʽ���������ݵľ��Ⱥʹ�С��
//t �Ӷ���������紫��Ч�ʣ������ڶ�����Ϸ��Ƶ��ͬ����λ�úͷ������ݡ�
void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize TraceHitTarget)
{
	if (EquipedWeapon == nullptr) return;
	if (Character )
	{
		Character->PlayFireMontage(bAiming);
		
		EquipedWeapon->Fire(TraceHitTarget);
	}
}

// Զ��RPC����
void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}
	

// ����HUD����,��ʼ������
void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (Character == nullptr || Character->Controller==nullptr) return;

	// Controller������Զ���������ı���������������û��ֵ�Ͱѽ�ɫ��Ĭ�Ͽ�������ֵ����
	Controller=Controller == nullptr ? Cast<AZeusPlayerController>(Character->GetController()) : Controller;


	if (Controller)
	{
		// APlayerController��ר�����ڴ���������롢������ƺ�HUD�������
		// HUD ����ʾ����Ļ�ϵ�ͼ�ν���Ԫ�أ�������ҵĽ���������������ҩ����Ϣ��
		HUD = HUD == nullptr ? Cast<AZeusHUD>(Controller->GetHUD()) : HUD;
		if (HUD)
		{
			
			if (EquipedWeapon)
			{
				
				HUDPackage.CrosshairsCenter = EquipedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquipedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquipedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsTop = EquipedWeapon->CrosshairsTop;
				HUDPackage.CrosshairsBottom = EquipedWeapon->CrosshairsBottom;

				
			}
			else
			{
				
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;

				
			}
			// �ڵ��û���׼��hud֮ǰ������׼�Ƿ�ɢֵ,���ݽ�ɫ���ٶ�ֵ
			// ����һ����ά������0.f�������ٶȵ���Сֵ��MaxWalkSpeed�����ٶȵ����ֵ��
			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			// ӳ�䵽׼�Ƿ�ɢ�ķ�Χ
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			// ��ȡ��ɫ��ˮƽ�����ٶ�
			FVector Vectory = Character->GetVelocity();
			Vectory.Z = 0;

			// ����ɫ��ǰ�ٶȴ�WalkSpeedRangeӳ�䵽VelocityMultiplierRange
			// �����ɫ����������ٶ���600�������ǰ�ٶ���300��Vectory.Size() ����300��
			// FMath::GetMappedRangeValueClamped ��300�ӷ�Χ [0, 600] ӳ�䵽 [0, 1]������� 0.5��CrosshairVelocityFactor ���Ϊ 0.5
			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Vectory.Size());

			if (Character->GetCharacterMovement()->IsFalling())
			{
				//t ��ֵ��������ƽ���ع���ĳ��ֵ��Ŀ��ֵ��
				//t ����1��ǰֵ��CrosshairInAirFactor������ǰ��׼������ֵ������2Ŀ��ֵ�����磬2.25f �� 0.f����������ֵ����Ŀ��ֵ��
				//t DeltaTime��ʱ����������ȷ����ֵ���̵�ƽ���ԡ�
				//t ��ֵ�ٶȣ�2.25f �� 30.f������ֵ���ٶȣ���ֵԽ�󣬹���Խ�졣
				//t ���Ǹ��ݲ�ֵ�ٶȺ�֡�ʣ���CrosshairInAirFactorֵ���ɵ�2.25����ֵ�ٶ�Խ�죬����Ҳ��Խ��
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
			}

			if (bAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, -0.58f, DeltaTime, 30.f);
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
			}

			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 40.f);

			HUDPackage.CrosshairSpread = 
				0.5f+
				CrosshairVelocityFactor+
				CrosshairInAirFactor+
				CrosshairAimFactor+
				CrosshairShootingFactor;

			HUD->SetHUDPackeage(HUDPackage);
		}
	}
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquipedWeapon == nullptr) return;
	if (bAiming)
	{
		// ��ȡ��ǰ��������Ұֵ�Ͳ�ֵ�ٶȣ��õ�ǰ��Ұֵƽ���Ĺ��ɵ���ǰ��������Ұֵ
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquipedWeapon->GetZoomedFOV(), DeltaTime, EquipedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		// ��������׼��ʱ��ƽ�����ɵ�Ĭ�ϵ���Ұֵ
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}

	if (Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}


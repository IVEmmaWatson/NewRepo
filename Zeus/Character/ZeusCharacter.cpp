// Fill out your copyright notice in the Description page of Project Settings.


#include "ZeusCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Zeus/Weapon/Weapon.h"
#include "Zeus/ZeusComponents/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Zeus/Character/ZeusAnimInstance.h"
#include "Zeus/Zeus.h"

AZeusCharacter::AZeusCharacter()
{
	// ���д����������û���� Actor �� Tick ����
	PrimaryActorTick.bCanEverTick = true;

	// ��������ʼ��һ�� USpringArmComponent�����ɱ������
	// CameraBoom ��һ��ָ���´����� USpringArmComponent ��ָ�롣
	// CreateDefaultSubobject ��һ��ģ�庯�������ڴ���һ���µ��Ӷ����������� USpringArmComponent ���ͣ�
	//  TEXT("CameraBoom") �������������ƣ����ڱ�ʶ�͵��ԡ�
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	// �����ɱ�������ӵ���ɫ�Ĺ�������ϣ�������CameraBoom �������ӵ��������������ɫ�������ƶ�����ת��
	// GetMesh() ��һ��������ͨ�����ڷ��ؽ�ɫ�����������USkeletalMeshComponent���������������ǽ�ɫ�Ŀ��ӻ���ʾ�������˽�ɫ�Ĺ����Ͷ������ݡ�
	CameraBoom->SetupAttachment(GetMesh());
	// ���ɱ۵ĳ���
	CameraBoom->TargetArmLength = 600.f;
	// ʹ���ɱ�ʹ�ý�ɫ�Ŀ�����ת������ζ��������������ɫ����ת����ͨ�����ڵ����˳��ӽǣ�ʹ�����ʼ�ն�׼��ɫ��ǰ����
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// �󶨵����ɱ��ϣ�Ĭ�ϲ���ϣ����ɱ������USpringArmComponent��ͨ��ֱ�Ӹ��ӵ���ɫ�ĸ��������������ϣ�������Ҫָ���ض��Ĳ�ۡ�
	// ���ɱ��������û�ж����ۣ�����Ĭ����Ϊ�ǽ����ӵ��������������������ڵ��ɱ۵�ĩ�ˡ�
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// ��ɫ���������һ����ת
	// ���������Ϊ false ʱ����ɫ������ݿ���������ת����������ĳ���Ҳ����˵����ɫ�ĳ�����ֱ�Ӹ������������ת��
	bUseControllerRotationYaw = false; 
	// ���������Ϊ true ʱ����ɫ������ƶ���������������ĳ���Ҳ����˵����ɫ���Զ��������ƶ��ķ���
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// ������ʾ�ı��ؼ�ʵ�����󶨿ؼ��������
	OverHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));
	OverHeadWidget->SetupAttachment(RootComponent);

	// ����ս���������ʵ�������󶨵����������ΪҪ�󶨵���ɫ�ֲ������
	// ս���ཫΪ���ǵĽ�ɫ����������ս����صĹ��ܣ�����������ʵ����ͨ�����ʵ�����Ե���ս��������б����ͷ���
	// component�������Ҫע��Ϊ���縴�����ԣ����÷������ü���
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	// ���д������� CombatComponent Ϊ�ɸ��Ƶģ�����ζ������״̬���ڷ������Ϳͻ���֮��ͬ����
	Combat->SetIsReplicated(true);

	// ͨ���� bCanCrouch ����Ϊ true���������ɫִ�ж��¶�����
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	// ���ù��������ײͨ������
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()-> SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	// ��ɫ��ת���ٶ�
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 100.f);


	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
}


void AZeusCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// ֻ�а�W�Żᴥ����������������㵥������겻���ȡ��ǰ��yawֵ��ֻ�а���W�Ż��ȡ��ʱ����yawֵ
void AZeusCharacter::MoveForward(float Value)
{
	// Controller�Ǵ�Apwan��̳����ģ���ʾ��ǰ���������ɫ�Ŀ�����
	if (Controller != nullptr && Value != 0.f)
	{
		// Controller->GetControlRotation().Yaw��ȡ��ҿ������ĵ�ǰ������ת�� Yaw ֵ��
		// �Ե�ǰyawֵ������������ֵ����һ����ת�������Ϊyaw������ҵ�ˮƽ��תҲ���ǽ�ɫ���򣬰�����ֵ�̶�ס

		// ����Ǵ���һ���Ի�ȡ����������yawֵΪ���ģ������Ǻ�roll�ǹ̶�Ϊ0 ����ת����
		// Ȼ�������������ת��������һ����ת���󣬻�ȡ��������X���ֵ
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		// FRotationMatrix(YawRotation)��ʹ�� YawRotation ����һ����ת����
		// GetUnitAxis(EAxis::X)������ת�����л�ȡ X ��ĵ�λ������
		// FVector Direction��������洢��һ�� FVector �����У���ʾ������������ΪX��ʼ�ձ�ʾ��ɫ��ǰ��
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		// UE_LOG(LogTemp, Warning, TEXT("Direction: X=%f, Y=%f, Z=%f"), Direction.X, Direction.Y, Direction.Z);

		// UE_LOG(LogTemp, Warning, TEXT("YawRotation: X=%f, Y=%f, Z=%f"), YawRotation.Roll, YawRotation.Yaw, YawRotation.Pitch);
		// AddMovementInput �� ACharacter ���е�һ���������������ɫ����ƶ����롣
		// ������������Ǹ���ָ���ķ��������ֵ���ƶ���ɫ��
		// value��ֵ��ʾ��ǰ����ֵ��ʾ���
		AddMovementInput(Direction, Value);

	}

	
}

void AZeusCharacter::MoveRight(float Value)
{
	// Controller�Ǵ�Apwan��̳����ģ���ʾ��ǰ���������ɫ�Ŀ�����
	if (Controller != nullptr && Value != 0.f)
	{
		
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

		// AddMovementInput �� ACharacter ���е�һ���������������ɫ����ƶ����롣
		// ������������Ǹ���ָ���ķ��������ֵ���ƶ���ɫ��
		// ��ֵ��ʾ���ң���ֵ��ʾ����
		AddMovementInput(Direction, Value);
	}
}

void AZeusCharacter::Turn(float Value)
{
	// ���ҿ�������value����ˮƽ��ת�ĽǶ�ֵ
	// Value ������ʾ��ת�ĽǶ�ֵ�����ֵͨ���Ǵ������豸�����������������ȡ�ġ�
	// ���磬�ƶ�������ҡ�˻����һ����Ӧ������ֵ��
	AddControllerYawInput(Value);

	
}

void AZeusCharacter::LookUp(float Value)
{
	// ���¿������Ƹ����ӽǵĽǶ�
	AddControllerPitchInput(Value);
}



void AZeusCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*
	// ÿ֡���ָ���Ƿ�Ϊ�գ���Ϊ���ָ��ֻ�������ˣ�û�ж��帳ֵ�������帳ֵ����SetOverlappingWeapon�������
	// �������������weapon�������������ײ���Ż���¸�ֵ�ģ�
	// ��Ϊtick������ÿ���ͻ��˺ͷ��������ᴥ���ļ�⣬��������ᵼ�����пͻ��˶�������ʾ�ı�����
	// ������ֻ��Ҫ˭��������ʾ��˭�ͻ����ϣ�������ʾ�����пͻ�����
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	*/
	AimOffset(DeltaTime);

	HideCameraIFCharacterClose();
}


// PlayerInputComponent->BindAxis�������Զ������������ֵ�����ð󶨵ĺ�����
// ���������Ϸ���ƶ�����ʹ�ÿ���������ҡ��ʱ��Unreal Engine���Զ������Щ���룬������Ӧ��ֵ���ݸ�Turn�����е�Value������
// ��ˣ��㲻��Ҫ�ֶ�����Turn�����򴫵ݲ�����Unreal Engine�������ҵ������Զ������Щ������
void AZeusCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// ����Ծ���� IE_Pressed������һ��ö��ֵ����ʾ�����¼������͡������IE_Pressed ��ʾ����������ʱ������
	// �����õ��Ǹ����Դ���jump������û������
	// �����һ�������Ǳ༭�������õ��ַ�������
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AZeusCharacter::Jump);
	// PlayerInputComponent����һ�������������������ҵ����롣
	//  /this����ǰ��ʵ�������ָ�룬��ǰ���Ǹ���ɫ�࣬���Կ��ƽ�ɫ
	// ���� PlayerInputComponent ��һ�����������ڰ������롣"MoveForward"����������������ƣ�ͨ������Ŀ�����ж��塣
	PlayerInputComponent->BindAxis("MoveForWard", this, &ThisClass::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ThisClass::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ThisClass::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ThisClass::LookUp);

	// ��װ��������ť
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ThisClass::EquipButtonPressed);
	// ���¶׶���
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ThisClass::CrouchButtonPressed);
	// ������Ҽ���׼����,һ������һ���ͷ�
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ThisClass::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ThisClass::AimButtonReleased);

	// ��������������
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ThisClass::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ThisClass::FireButtonReleased);
}

// GetLifetimeReplicatedProps����ע��һ���������ͬ������
// ����������Ҫ��ĳ�� Actor ������ͬ�����ͻ���ʱ����������ᱻ������ȷ����Щ������Ҫͬ���Լ����ͬ����
void AZeusCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// �� AZeusCharacter ���е� OverlappingWeapon ����ע��Ϊ��Ҫ�������Ͻ��и��Ƶ����ԣ�
	// ��ȷ���ڿͻ��˺ͷ�����֮��ͬ�������Ե�ֵ��
	// DOREPLIFETIME(AZeusCharacter, OverlappingWeapon);
	// COND_OwnerOnly: ���Ǹ�����������ʾ�ñ���ֻ�Ḵ�Ƹ�ӵ�иý�ɫ�Ŀͻ��ˡ�
	DOREPLIFETIME_CONDITION(AZeusCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void AZeusCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
}


void AZeusCharacter::OnRep_OverlappingWeapon(AWeapon *LastWeapon)
{

	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}


	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
	
}



void AZeusCharacter::EquipButtonPressed()
{
	if (Combat )
	{
		// ��Ȩ���ı�ʾ��������ִ�еİ�E��
		if (HasAuthority())
		{
			// NOTE: ֻ�е���ɫ������ײ����ʱ���OverlappingWeaponָ��Ż���ֵ��������combat�ﲻ�ü�������ͽ�ɫ����ײ�¼�
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			// ûȨ�����ǿͻ��˰�E��
			ServerEquipButtonPressed();
		}
	}
}





// _Implementation ��׺��ʾ�����ڷ�������ִ�еĺ���ʵ��.�ͻ��˲���Ҫд����߼�����ֻ�õ��þ���
// �ͻ��˵��� ServerEquipButtonPressed ����ʱ��Unreal Engine �Ὣ������������͵���������
// _Implementation ��׺�������׺���ڶ���������˵�ʵ���߼������ͻ��˵����������ʱ��Unreal Engine ���ڷ�������ִ�д��� _Implementation ��׺�ĺ�����
void AZeusCharacter::ServerEquipButtonPressed_Implementation()
{
	// HasAuthority()����Ҫ������Ȩ������Ϊ�������ֻ�ڷ�����ִ��
	if (Combat)
	{
		// ֻ�е���ɫ������ײ����ʱ���OverlappingWeaponָ��Ż���ֵ��������combat�ﲻ�ü�������ͽ�ɫ����ײ�¼�
		Combat->EquipWeapon(OverlappingWeapon);
	}
}




void AZeusCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{

	// һ��ʼOverlappingWeaponΪ�գ�����������ִ�У��ڶ��ε���ʱOverlappingWeapon����ֵ�ˣ�
	// ����������ִ���ˣ�ִ�к��ְ�OverlappingWeapon��Ϊ����
	
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	
	OverlappingWeapon = Weapon;

	
	// ��鵱ǰ Actor �Ƿ��ɱ��ؿͻ��˿��ƣ���Ϊ�������Ƿ��������ǿͻ���
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

bool AZeusCharacter::IsWeaponEquipped()
{
	// ����Ƿ�װ��������,���ָ���Ƿ��ָ�룬��ΪEquipedWeapon������ǿմ����Ѿ�װ��������
	return (Combat && Combat->EquipedWeapon);
}

bool AZeusCharacter::IsAiming()
{
	// ��鲼��ֵ���Ƿ�����׼״̬
	return (Combat && Combat->bAiming);
}

AWeapon* AZeusCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr)return nullptr;
	return Combat->EquipedWeapon;
}

FVector AZeusCharacter::GetHitTarget() const
{
	if (Combat == nullptr)return FVector();
	return Combat->HitTarget;
}


void AZeusCharacter::CrouchButtonPressed()
{
	// ����Ƿ��Ѿ��Ƕ���״̬
	if (bIsCrouched)
	{
		UnCrouch();
		// bIsCrouched�Ƿ��ڶ���״̬���Ǹ�����ֵ�����õ�
		// ��UnCrouch��Crouch�Ǹı��������ֵ��
	}
	else
	{
		Crouch();
	}
}

void AZeusCharacter::AimButtonPressed()
{
	// ��ʵ���ս�����ָ��һֱ����ֵ
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void AZeusCharacter::AimButtonReleased()
{
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void AZeusCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquipedWeapon == nullptr) return;
	FVector Velocity =GetVelocity();
	Velocity.Z = 0.f;
	float  Speed = Velocity.Size();

	bool bIsInAir = GetCharacterMovement()->IsFalling();

	// վ�Ų�����������Ծ״̬
	//t 1.����ɫ�ƶ���ֹͣʱ��StartingAimRotationֵ�����ֹͣ�ƶ�����һ֡��ֵ����ȻҲ���������ƶ�ֵ
	//t �����ɫ�ƶ�����ֹͣ����˫��AFK����ʱ��StartingAimRotation��120.f�����Ҳ����GetBaseAimRotation()��Ҳ����CurrentAimRotation��ֵҲ��120.f
	//t ������Ϊ�����תֵ�Ǹ���yawֵȥ���Ķ��������ģ�������ֵһ�����������0yaw,�Ǿ��ǻ������ƣ�Ȼ�����ת����yawֵ�ͱ��˶���Ҳ���ˡ��ܼ�
	if (Speed == 0.f && !bIsInAir)
	{
		// ��ȡ��ǰ�������תֵ��yawֵ
		FRotator CurrentAimRotation= FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		// ���������תֵ�ͽ�ɫ�ƶ���ͣ������ֵ�Ĳ��죬Ҳ����yawֵ�Ĳ�ֵ
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation,StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		// ���ÿ�������ת����ɫ��������ת��
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAo_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);		//! �������deltatime�������Բ��ӣ�Ӧ����Ԥ�л������������û�õ���ֱ�ӵ��������������
		// UE_LOG(LogTemp, Warning, TEXT("yes start yaw  %f"), StartingAimRotation.Yaw);
		// UE_LOG(LogTemp, Warning, TEXT("asd yaw  %f"),AO_Yaw);
		// UE_LOG(LogTemp, Warning, TEXT("base yaw  %f"), CurrentAimRotation.Yaw);
	}


	if (Speed > 0.f || bIsInAir)
	{
		// ���½�ɫ�ƶ�ʱ�����ʱ����תֵ����ʵֻ�ý�ɫͣ��������һ֡,ʼ�ջ�ȡ��ɫ���������																		// 		Ϊʲô��ֵ�ܱ�ʾ��׼ƫ��
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);																//		����ɫֹͣ�ƶ�ʱ��StartingAimRotation ��¼�˽�ɫֹͣʱ����׼����
		// �ر���׼ƫ��																																					//		֮���κ�����ƶ�����ı� CurrentAimRotation���� StartingAimRotation ���ֲ��䡣
		AO_Yaw = 0.f;																																					 	//		ͨ������ CurrentAimRotation �� StartingAimRotation ֮��Ĳ�ֵ�����ǿ��Եõ���ɫ�ھ�ֹ״̬�µ���׼ƫ�������� AO_Yaw��
		// ���ÿ�������ת����ɫ������ת��																													//		�����ɫֹͣʱ�� StartingAimRotation Ϊ 30 �ȣ�Yaw������ʱ AO_Yaw Ϊ 0��
		bUseControllerRotationYaw = true;																													//		�������ƶ�ʹ CurrentAimRotation ��Ϊ 60 �ȣ�Yaw������ DeltaAimRotation.Yaw Ϊ 60 - 30 = 30 �ȣ�AO_Yaw Ϊ 30 �ȡ�
		// UE_LOG(LogTemp, Warning, TEXT("no start yaw  %f"), StartingAimRotation.Yaw);
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	
	}																																												//		���ʾ��ɫ�ھ�ֹ״̬�£���׼���������ֹͣʱ�ķ���ƫ���� 30 �ȡ�

	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// ����һ����ά���� InRange����ʾ���뷶ΧΪ 270 �� 360 �ȡ�
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		// ʹ�� FMath::GetMappedRangeValueClamped ������ AO_Pitch �� InRange ӳ�䵽 OutRange���������ȷ��ӳ����ֵ�������Χ�ڡ�Ҳ����˵��
		// ��� AO_Pitch �� 270 �� 360 ��֮�䣬���ᱻӳ�䵽 -90 �� 0 ��֮�䡣
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);;
	}
	
}

void AZeusCharacter::Jump()
{
	
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}

}


// ������ɫת������ߺ��ͻȻת�����ұߣ���������һ�£���ɫת�������ʱ�������м�Ҳ���ǽ�ɫ��ǰ��
void AZeusCharacter::TurnInPlace(float DeltaTime)
{
	// UE_LOG(LogTemp, Display, TEXT("Ao_yaw:%f"),AO_Yaw);
	if (AO_Yaw > 60.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_RIght;
	}
	else if (AO_Yaw<-90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAo_Yaw = FMath::FInterpTo(InterpAo_Yaw, 0.f, DeltaTime, 5.f);
		AO_Yaw = InterpAo_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}


void AZeusCharacter::ServerHit_Implementation()
{
	MulticastHit();
}


void AZeusCharacter::MulticastHit_Implementation()
{
	PlayHitReactMontage();
}

void AZeusCharacter::HideCameraIFCharacterClose()
{

	// ����Ƿ�Ϊ���ؿ��ƵĽ�ɫ��������ǣ���ֱ�ӷ��ز�ִ�к���������������ȷ��ֻ�б��ؿ��ƵĽ�ɫִ�пɼ��Ը���
	if (!IsLocallyControlled())return;

	// (FollowCamera->GetComponentLocation() - GetActorLocation()).Size() ����������ͽ�ɫ��λ�þ��롣
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquipedWeapon && Combat->EquipedWeapon->GetWeaponMesh())
		{
			// bOwnerNoSee ����Ϊ true����ʾ��ɫ�Լ���������������
			Combat->EquipedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquipedWeapon && Combat->EquipedWeapon->GetWeaponMesh())
		{
			Combat->EquipedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void AZeusCharacter::FireButtonPressed()
{
	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void AZeusCharacter::FireButtonReleased()
{
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

void AZeusCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquipedWeapon == nullptr) return;

	// ���д����ȡ��ɫ�Ķ���ʵ����UAnimInstance�������ڿ��ƶ����Ĳ��š�
	// ����ʵ�����������Ÿ��ӵĶ������У��򵥵Ķ�������ֱ���ù����������playanimantion
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	// ��� AnimInstance �� FireWeaponMontage �Ƿ���Ч�������Ч���򲥷� FireWeaponMontage ������̫�档
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		// ��δ������ bAiming ����ѡ��Ҫ���ŵĶ����Ρ���� bAiming Ϊ true����ѡ�� "RifleAim" �Σ�
		// ����ѡ�� "RifleHip" �Ρ�Ȼ��ʹ�� Montage_JumpToSection ������ת��ָ���Ķ����Ρ�
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AZeusCharacter::PlayHitReactMontage()
{

	if (Combat == nullptr || Combat->EquipedWeapon == nullptr) return;

	// ���д����ȡ��ɫ�Ķ���ʵ����UAnimInstance�������ڿ��ƶ����Ĳ��š�
	// ����ʵ�����������Ÿ��ӵĶ������У��򵥵Ķ�������ֱ���ù����������playanimantion
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	// ��� AnimInstance �� FireWeaponMontage �Ƿ���Ч�������Ч���򲥷� FireWeaponMontage ������̫�档
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");
		// ��δ������ bAiming ����ѡ��Ҫ���ŵĶ����Ρ���� bAiming Ϊ true����ѡ�� "RifleAim" �Σ�
		// ����ѡ�� "RifleHip" �Ρ�Ȼ��ʹ�� Montage_JumpToSection ������ת��ָ���Ķ����Ρ�
		
		AnimInstance->Montage_JumpToSection(SectionName);
	}


}



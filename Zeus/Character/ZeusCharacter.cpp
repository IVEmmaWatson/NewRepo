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
}


void AZeusCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AZeusCharacter::MoveForward(float Value)
{
	// Controller�Ǵ�Apwan��̳����ģ���ʾ��ǰ���������ɫ�Ŀ�����
	if (Controller != nullptr && Value != 0.f)
	{
		// Controller->GetControlRotation().Yaw��ȡ��ҿ������ĵ�ǰ������ת�� Yaw ֵ��
		// �Ե�ǰyawֵ������������ֵ����һ����ת�������Ϊyaw������ҵ�ˮƽ��תҲ���ǽ�ɫ���򣬰�����ֵ�̶�ס
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		// FRotationMatrix(YawRotation)��ʹ�� YawRotation ����һ����ת����
		// GetUnitAxis(EAxis::X)������ת�����л�ȡ X ��ĵ�λ������
		// FVector Direction��������洢��һ�� FVector �����У���ʾ������������ΪX��ʼ�ձ�ʾ��ɫ��ǰ��
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));

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
	AddControllerYawInput(Value);
}

void AZeusCharacter::LookUp(float Value)
{
	// ���¿������Ƹ����ӽǵĽǶ�
	AddControllerPitchInput(Value);
}

void AZeusCharacter::EquipButtonPressed()
{
	if (Combat&&HasAuthority())
	{
		// ֻ�е���ɫ������ײ����ʱ���OverlappingWeaponָ��Ż���ֵ��������combat�ﲻ�ü�������ͽ�ɫ����ײ�¼�
		Combat->EquipWeapon(OverlappingWeapon);
	}
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
}


void AZeusCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// ����Ծ���� IE_Pressed������һ��ö��ֵ����ʾ�����¼������͡������IE_Pressed ��ʾ����������ʱ������
	// �����õ��Ǹ����Դ���jump������û������
	// �����һ�������Ǳ༭�������õ��ַ�������
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	// PlayerInputComponent����һ�������������������ҵ����롣
	// this����ǰ��ʵ�������ָ�룬��ǰ���Ǹ���ɫ�࣬���Կ��ƽ�ɫ
	// ���� PlayerInputComponent ��һ�����������ڰ������롣"MoveForward"����������������ƣ�ͨ������Ŀ�����ж��塣
	PlayerInputComponent->BindAxis("MoveForWard", this, &ThisClass::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ThisClass::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ThisClass::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ThisClass::LookUp);

	// ��װ��������ť
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ThisClass::EquipButtonPressed);
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



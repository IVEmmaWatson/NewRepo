// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ZeusCharacter.generated.h"

UCLASS()
class ZEUS_API AZeusCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AZeusCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();

public:	
	// Called every frame
	// �������ÿһ֡���ᱻ���ã����ڸ��¶����״̬��DeltaTime ������ʾ����һ֡����������ʱ�䣨����Ϊ��λ����
	// �����ʵ��ƽ���Ķ���������Ч���ǳ���Ҫ��
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	// ����������ڰ�������뵽�ض��Ĺ����ϡ�PlayerInputComponent ��һ�����������
	// �������㽫���������¼����簴��������ƶ��ȣ��󶨵���Ӧ�Ĵ������ϡ�
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// �����������Ҫ�����ǽ���Ҫ�������Ͻ��и��Ƶ�������ӵ� OutLifetimeProps ������
	// TArray<FLifetimeProperty>ģ�������鶨�壬��FLifetimeProperty���͵õ�����
	// FLifetimeProperty ��һ���ṹ�壬����������Ҫ���Ƶ����ԡ�
	// OutLifetimeProps��OutLifetimeProps ��һ�����ã�ָ��һ�� TArray<FLifetimeProperty> ���͵�����
	// const ����ؼ��ֱ�ʾ���������޸���ĳ�Ա������s
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// ���������������ʼ����ɺ���ж���ĳ�ʼ����������beginpaly֮ǰ������
	virtual void PostInitializeComponents() override;

private:
	// ������������ԣ�����Ʊ����ڱ༭���еĿɼ��ԡ��ɱ༭�ԣ��Լ���������ͼ�еķ���Ȩ��
	// ����VisibleAnywhere���ڱ༭���пɼ��������ɱ༭��Category�����������ൽ�ض�����У������ڱ༭������֯�Ͳ��ҡ�
	UPROPERTY(VisibleAnywhere, Category = Camera)
	// ����������ͽ�ɫֱ�ӵ�֧��
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	// ���ø����ɫ�ƶ��������
	class UCameraComponent* FollowCamera;
	
	// ����һ���û��ؼ������ָ�룬�����ڱ༭���б༭����������ͼ�ж�ȡ�����ԣ�������˽�г�Ա������ʹ��UPROPERTY�꣬ʹ���ڱ༭������ͼ�пɼ���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess="true"))
	class UWidgetComponent* OverHeadWidget;

	// ����һ��ָ�� AWeapon ������ָ�� OverlappingWeapon����ʹ���������Ͻ��и��ƣ���ȷ���ͻ��˺ͷ�����֮�������һ���ԡ�
	// ReplicatedUsing = OnRep_OverlappingWeapon: ������Ա�ʾ���� OverlappingWeapon �ڷ������Ϸ����仯ʱ��
	// �ͻ��˻���� OnRep_OverlappingWeapon ������
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	// ReplicatedUsing ���ԵĹ�������
	// ����ʹ�� UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon) ���һ������ʱ
	// �ڵ��� OnRep_OverlappingWeapon ����ʱ��Unreal Engine �Ὣ OverlappingWeapon �����仯ǰ��ֵ����֮ǰ������ָ�룩
	// ��Ϊ�������ݸ� OnRep_OverlappingWeapon ������ LastWeapon ������
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);


	// ս�������ָ�룬�ڽ�ɫ��ͼ���������
	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;

	// Զ�̹��̵���RPC��Server��������η���ʾ�������ڷ�������ִ�С��ͻ��˵����������ʱ��Unreal Engine �Ὣ���������͵������������������յ������ִ�иú�����
	// Reliable��������η���ʾ���������ǿɿ��ģ�ȷ�����ò��ᶪʧ����ʹ����״�����ѣ�Unreal Engine Ҳ�����Է��ͣ�ֱ���������ɹ����յ���ִ�С�
	// ��������Ҫ�����������ִ�д��롣������˵�������ڿͻ��˵���һ������ Server ��ǵĺ���ʱ��Unreal Engine �Ὣ������������͵������������ڷ�������ִ����Ӧ��ʵ�ִ��롣
	UFUNCTION(Server,Reliable)
	void ServerEquipButtonPressed();



public:
	// ���縴�Ƶı�����ֻ���ڷ������ϵ�������ķ����仯ʱ�Ż���ͻ���ͨ�ţ����߿ͻ��˸����Ա仯��
	// ����ֻ��֪ͨ�˿ͻ������Ըı��ˣ���û��֪ͨ��������Replicated���縴�ƵĹ�����ʽֻ���ڴӷ�����֪ͨ�ͻ���
	// FORCEINLINE void SetOverlappingWeapon(AWeapon* Weapon) { OverlappingWeapon = Weapon; };
	void SetOverlappingWeapon(AWeapon* Weapon);
	// ��ʾ�Ƿ���״̬����״̬
	bool IsWeaponEquipped();
	// ��ʾ�Ƿ���������׼״̬
	bool IsAiming();
};

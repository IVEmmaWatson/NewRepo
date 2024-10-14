// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Zeus/ZeusTypes/TurningInPlace.h"
#include "Zeus/interfaces/InteractWithCrosshairInterface.h"
#include "Components/TimelineComponent.h"
#include "ZeusCharacter.generated.h"


// AZeusCharacter ���̳��� IInteractWithCrosshairInterface �ӿڡ�����ζ�� AZeusCharacter �����ʵ�� IInteractWithCrosshairInterface �ӿ��е����з���������ж��壩��
UCLASS()
class ZEUS_API AZeusCharacter : public ACharacter,public IInteractWithCrosshairInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AZeusCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void UpdateHUDHealth();

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn();
	virtual void Jump() override;
	void FireButtonPressed();
	void FireButtonReleased();
	float CalculateSpeed();

	// �����˺�
	UFUNCTION()
	void ReceiveDamage(AActor* DamgedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
	// ��ɫ����ֵ
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere,Category = "Player Stats")
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health();

	class AZeusPlayerController* ZeusPlayerController;

	bool bElimmed = false;

	FTimerHandle ElimTimer;

	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;

	void ElimTimerFinished();

	/**
	* ��ɫ�ܽ���Ч
	* �Զ����ʱ�����¼���Timeline Event�����ͣ����ڴ�������ֵ��ʱ����켣�仯������Ϸ�����У��������͵ı���ͨ�����ڶ�����Ч���Ŀ��ơ�
	* FOnTimelineFloat����һ��ί�����ͣ�Delegate Type��������ʱ�����и�����ֵ�仯ʱ���¼���Ӧ��
	* ������һ��������ʱ�����¼�ί�У����ڴ���ʱ�����еĸ�����ֵ�仯��
	* UTimelineComponentʱ���������������һ��ʱ�䷶Χ�ڿ��Ƹ���������������ɫ�ȱ�����ͨ�����ڶ�����Ч����ʵ�֡�
	*/
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;

	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();

	// Dynamic instance that we can change at runtime
	// ��̬����ʵ�����ɿ����ɸ�
	UPROPERTY(VisibleAnywhere, Category = "Elim")
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	// Material instance set on the Blueprint,used with the dynamic meterial instance
	// ��������ʵ�����ɿ����θ�
	UPROPERTY(EditAnywhere, Category = "Elim")
	UMaterialInstance* DissolveMaterialInstance;

	/**
	* ��̭������Ч
	* UParticleSystem��������Ч������ һ������ϵͳģ�壬�����ڱ༭�������ú�ѡ����������Ч����
	*  UParticleSystemComponent��������������ʵ�����͹�������ϵͳ��������������ڳ�����ʵ����ʾ�Ϳ�������Ч����
	*/
	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimBotEffect;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimBotComponent;

	UPROPERTY(EditAnywhere)
	class USoundCue* ElimBotSound;

	virtual void Destroyed() override;
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
	void PlayFireMontage(bool bAiming);
	void PlayHitReactMontage();
	
	void PlayElimMontage();
	/*
	UFUNCTION(Server, Reliable)
	void ServerHit();


	// NetMulticast����ʾ���������һ���ಥ����,�����ڷ���������ʱͬ�������пͻ��ˡ����仰˵���������������������ʱ���������ӵĿͻ��˶���ִ�����������
	// Unreliable����ʾ����ಥ�����ǲ��ɿ��ġ�Ҳ����˵������֤��������ĵ���һ���ᵽ��ͻ��ˣ����ܻᶪʧһЩ���á����ֲ��ɿ���ͨ��������ҪƵ�����£�������Ҫ��ȷͬ������������綯������Ч�ȡ�
	// ��ĳ����һ���Ŀ��ʱ���������Ե��� MulticastHit ��֪ͨ���пͻ��˲��Ż�����Ч,����Ҫÿ�ζ�ȷ��ÿ���ͻ��˶����յ����֪ͨ
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastHit();
	*/

	// ReplicatedMovement ͨ��������ɫ�������λ�á���ת���ٶȵ���Ϣ��
	// ����Щ�����ڷ������Ϸ����仯ʱ�������Ὣ��Щ�仯ͬ�����ͻ��ˡ�
	virtual void OnRep_ReplicatedMovement() override;
	void Elim();

	UFUNCTION(NetMulticast,Reliable)
	void MulticastElim();
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

	float AO_Yaw;
	float InterpAo_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ElimMontage;
	

	void HideCameraIFCharacterClose();

	UPROPERTY(EditAnywhere)
	float CameraThreshold=200.f;


	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;

	float TimeSinceLastMovementReplication;
public:
	// ���縴�Ƶı�����ֻ���ڷ������ϵ�������ķ����仯ʱ�Ż���ͻ���ͨ�ţ����߿ͻ��˸����Ա仯��
	// ����ֻ��֪ͨ�˿ͻ������Ըı��ˣ���û��֪ͨ��������Replicated���縴�ƵĹ�����ʽֻ���ڴӷ�����֪ͨ�ͻ���
	// FORCEINLINE void SetOverlappingWeapon(AWeapon* Weapon) { OverlappingWeapon = Weapon; };
	void SetOverlappingWeapon(AWeapon* Weapon);
	// ��ʾ�Ƿ���״̬����״̬
	bool IsWeaponEquipped();
	// ��ʾ�Ƿ���������׼״̬
	bool IsAiming();

	FORCEINLINE float GETAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GETAO_Pitch() const { return AO_Pitch; }

	AWeapon* GetEquippedWeapon();

	// ��ȡö�ٱ���ֵ
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }

	FVector GetHitTarget() const;

	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
};

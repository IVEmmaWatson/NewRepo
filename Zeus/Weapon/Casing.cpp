// Fill out your copyright notice in the Description page of Project Settings.
#include "Casing.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"

// Sets default values
ACasing::ACasing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);

	// ��������Ե��ǵ���ײ��Ӧ����Ȼ�����Ŵ󵰿��ӽ�
	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	// �������ǵ�����ģ�⣬����Ҫ��������ѭ�����ɣ������������塢��ײ����ʱ����Ҫ��������ģ�⡣
	CasingMesh->SetSimulatePhysics(true);
	//  ����������Ч��������Ҫ����������Ӱ��ʱ������Ͷ�����������ȣ���������������ǿ������ֵ���ʵ��
	CasingMesh->SetEnableGravity(true);
	// SetNotifyRigidBodyCollision(true) ʹ�� CasingMesh �ڷ���������ײʱ��������֪ͨ���Ӷ����� OnComponentHit ���¼���
	// ���������������ᴥ�� OnComponentHit �¼�������ζ���κ��� CasingMesh ����ײ�����������󶨵� OnHit ������
	// �ӵ�����Ҫ���ø�����ײ֪ͨ����Ϊ����Ҫ����������Ŀ�ꡣ
	CasingMesh->SetNotifyRigidBodyCollision(true);
	ShellEjectionImpulse = 5.f;
	
}

// Called when the game starts or when spawned
void ACasing::BeginPlay()
{
	Super::BeginPlay();
	
	// �󶨵��Ǳ������¼�,�����������ײʱ����
	CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);
	
	// ���д���Ϊ ����ʵ�� ���һ�����Ž�ɫǰ����ĳ�����
	// GetActorForwardVector(): ����������ص�ǰActor��ǰ����������ShellEjectionImpulse: �����ʼ�����Ĵ�С��
	// ��ǰ�����������Գ�ʼ�������õ�һ������ĳ���������
	// AddImpulse(): ����һ�� UPrimitiveComponent �ķ���������Ϊ�������˲ʱ������
	CasingMesh->AddImpulse(GetActorForwardVector()*ShellEjectionImpulse);
}

void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OhterActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
	}
	Destroy();
}





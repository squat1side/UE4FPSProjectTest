// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter():

	//��ʼ����������
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f)

{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//ʵ�ֽ������������ΪĬ������������������ڸ����,�����ñ۳�����������������ӽ�
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;
	//���ò��ƫ�� - ʹ��ɫ������Ļ�м�  -ʹ������������һЩƫ����
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 50.f);



	//����CameraFollow
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//����������ڵ��ɱ۵�ĩ�˲��
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//��������������ת
	FollowCamera->bUsePawnControlRotation = false;

	//������ֻӰ�����������Ӱ���ɫ��ת��
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	//���ý�ɫ���ƶ�
	GetCharacterMovement()->bOrientRotationToMovement = false;//��ɫ�����뷽�����ƶ�
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);//��ת�ٶ�
	//���ý�ɫ��Ծ
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

//ʵ��MoveForward
void AShooterCharacter::MoveForward(float Value)
{
	//���̳еĿ������ı����Ƿ���Ч
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		//�ҳ��Ǹ�������ǰ��{��ʼ��}
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		//����ƶ����� -�ƶ���ɫ���ƶ�����е�������Ϣȷ��
		AddMovementInput(Direction, Value);
	}
}

//ʵ��MoveRight
void AShooterCharacter::MoveRight(float Value)
{
	//���̳еĿ������ı����Ƿ���Ч
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		//�ҳ��Ǹ�����������ת��{��ʼ��}
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		//����ƶ����� -�ƶ���ɫ���ƶ�����е�������Ϣȷ��
		AddMovementInput(Direction, Value);
	}
}

//ʵ���ӽ�ת����Ӱ�������
void AShooterCharacter::TurnAtRate(float Rate)
{
	//����������Ϣ����ÿһ֡��Delta
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

//�������������ܣ�
void AShooterCharacter::FireWeapon()
{
	//�жϿ�����Ч�Ƿ���Ч
	if (FireSound)
	{
		//����play sound 2D -��̬���� - ��Ϸ��play�Ǿ�̬��
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
	//��ȡ��� - ����һ���ֲ�����
	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());
		//����������Ч
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		//����һ��F��������Ϊ����
		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(
			SocketTransform.GetLocation(), BeamEnd);
		if (bBeamEnd)
		{
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					BeamEnd);
			}
			//����������Ӻ󣬴�����������
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				BeamParticles,
				SocketTransform);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}

	//����һ���ֲ���������ʵ�� - ���ڿ�����̫���п��𶯻�
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

//�����ȡ������λ�õ�GetBeamEndLocation������
bool AShooterCharacter::GetBeamEndLocation(
	const FVector& MuzzleSocketLocation, 
	FVector& OutBeamLocation)
{
	//��ȡ��Ļ���ĵ�׼��λ�� - ��׼����������Ļλ������������׷��
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	//��ȡ��Ļ׼��λ�ã�����һ��F����
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	CrosshairLocation.Y -= 50.f;
	//Ϊ׼�ǵ�����λ�ú����緽�򴴽��ֲ�����
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	//��ȡ����λ�ú�׼�Ƿ���
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	//�ж�bScreenToWorld - ͶӰ�Ƿ�ɹ�
	if (bScreenToWorld)
	{
		FHitResult ScreenTraceHit;
		const FVector Start{ CrosshairWorldPosition };
		const FVector End(CrosshairWorldPosition + CrosshairWorldDirection * 50000.f);

		//����׷��
		OutBeamLocation = End;

		//׼�����߷���׷������λ��
		GetWorld()->LineTraceSingleByChannel(
			ScreenTraceHit,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);
		//���������н��
		if (ScreenTraceHit.bBlockingHit)
		{
			//����ָ������λ��
			OutBeamLocation = ScreenTraceHit.Location;

		}
		//����ǹ�ڵ����е�Ĺ켣 -ǹ���������֮��������壬��Ҫ�ı��ӵ�ײ���㣬ʹ��ײ���ڸ�������
		FHitResult WeaponTraceHit;
		const FVector WeaponTraceStart{ MuzzleSocketLocation };
		const FVector WeaponTraceEnd{ OutBeamLocation };
		GetWorld()->LineTraceSingleByChannel(
			WeaponTraceHit,
			WeaponTraceStart,
			WeaponTraceEnd,
			ECollisionChannel::ECC_Visibility);
		//����Ƿ������ڻ��е��ǹ��֮��Ķ���
		if (WeaponTraceHit.bBlockingHit)
		{
			OutBeamLocation = WeaponTraceHit.Location;
		}
		return true;
	}

	return false;
}



// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//ʹ��check������������Ƿ���Ч
	check(PlayerInputComponent);

	//���ƶ������󶨵���ӳ��
	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);

	//��ת�����ʺ����󶨵���ӳ��
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);

	//�����ת�����ʺ����󶨵���ӳ��
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	//��jump�������ڶ���ӳ��
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this,  &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//������������ڶ���ӳ��
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireWeapon);

}



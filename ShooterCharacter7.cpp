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
	BaseLookUpRate(45.f),
	//Aiming��ת������
	HipTurnRate(90.f),
	HipLookUpRate(90.f),
	AimingTurnRate(90.f),
	AimingLookUpRate(20.f),
	//�����������׼/����׼
	MouseHipTurnRate(1.0f),
	MouseHipLookUpRate(1.0f),
	MouseAimingTurnRate(0.2f),
	MouseAimingLookUpRate(0.2f),
	//��ʼ����׼����
	bAiming(false),
	//��ʼ��Ĭ����Ұ�ͷŴ�����Ұ(��)������Beginplay
	CameraDefaultFOV(0.f),
	CameraZoomedFOV(35.f),
	//��ʼ�������ٶ�
	CameraCurrentFOV(0.f),
	ZoomInterpSpeed(20.f),
	//׼�Ǹ���
	CrosshairSpreadMultiplier(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimFactor(0.f),
	CrosshairShootingFactor(0.f),
	//�ӵ������ʱ������
	ShootTimeDuration(0.05f),
	bFiringBullet(false)


{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//ʵ�ֽ������������ΪĬ������������������ڸ����,�����ñ۳�����������������ӽ�
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 180.f;
	CameraBoom->bUsePawnControlRotation = true;
	//���ò��ƫ�� - ʹ��ɫ������Ļ�м�  -ʹ������������һЩƫ����
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);



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
	
	//����������Ұ�����
	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
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


//�������X/Y�ƶ�����ת�������Ĳ���ֵ - ����ֵ��������ƶ�������ֵ
void AShooterCharacter::Turn(float Value)
{
	float TurnScaleFactor{};
	if (bAiming)
	{
		TurnScaleFactor = MouseAimingTurnRate;
	}
	else
	{
		TurnScaleFactor = MouseHipTurnRate;
	}
	AddControllerYawInput(Value * TurnScaleFactor);
}

void AShooterCharacter::LookUp(float Value)
{
	float LookScaleFactor{};
	if (bAiming)
	{
		LookScaleFactor = MouseAimingLookUpRate;
	}
	else
	{
		LookScaleFactor = MouseHipLookUpRate;
	}
	AddControllerPitchInput(Value * LookScaleFactor);
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
	//Ϊ׼�Ǻ�ʱ��ʼ��������ӵ�
	StartCrosshairBulletFire();

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
	//CrosshairLocation.Y -= 50.f;
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

//Ϊ��׼������λ���������»��ɿ���
void AShooterCharacter::AimingButtonPressed()
{
	bAiming = true;
}

void AShooterCharacter::AimingButtonReleased()
{
	bAiming = false;
}

//�����ع��ͼ�Tick�����в�ֵ��Ұ����
void AShooterCharacter::CameraInterpZoom(float DeltaTime)
{
	//����Ƿ�����׼ -���õ�ǰ���������Ұ
	if (bAiming)
	{
		//��ֵ��ZoomedFOV
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraZoomedFOV,
			DeltaTime,
			ZoomInterpSpeed);
	}
	else
	{
		//��ֵ��DefaultFOV
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraDefaultFOV,
			DeltaTime,
			ZoomInterpSpeed);
	}
	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
}

void AShooterCharacter::SetLookRates()
{
	//����Aiming����ת����
	if (bAiming)
	{
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AimingLookUpRate;
	}
	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}
}

//ʵ��׼�Ǹ�����ÿ֡����
void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange{ 0.f, 600.f };
	FVector2D VelocityMultiplierRange{ 0.f, 1.f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f;

	//����׼�ǵĸ����ٶ�
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(
		WalkSpeedRange,
		VelocityMultiplierRange,
		Velocity.Size());

	//���м���Ƿ��ڿ���  - ׼�Ǹ����ڿ���
	if (GetCharacterMovement()->IsFalling())
	{
		//�ڿ���ʱ��׼�ǵ���ɢ
		CrosshairInAirFactor = FMath::FInterpTo(
			CrosshairInAirFactor,
			2.25f,
			DeltaTime,
			2.25f);
	}
	else//����ɫ�ڵ�����
	{
		//�ڵ����Ͽ�����С׼��
		CrosshairInAirFactor = FMath::FInterpTo(
			CrosshairInAirFactor,
			0.f,
			DeltaTime,
			30.f);
	}

	//��׼����ʱ���׼�ǵĸ���
	if (bAiming)
	{
		//�ڶ�ʱ�ڿ�����С׼��
		CrosshairAimFactor = FMath::FInterpTo(
			CrosshairAimFactor,
			0.6f,
			DeltaTime,
			30.f);
	}
	else
	{
		//׼�ǿ��ٻظ�����
		CrosshairAimFactor = FMath::FInterpTo(
			CrosshairAimFactor,
			0.f,
			DeltaTime,
			30.f);
	}

	//��0.05s���
	if (bFiringBullet)
	{
		CrosshairShootingFactor = FMath::FInterpTo(
			CrosshairShootingFactor,
			0.3f,
			DeltaTime,
			60.f);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(
			CrosshairShootingFactor,
			0.f,
			DeltaTime,
			60.f);
	}


	//׼�Ǹ����ļ���
	CrosshairSpreadMultiplier = 
		0.5f + 
		CrosshairVelocityFactor + 
		CrosshairInAirFactor - 
		CrosshairAimFactor +
		CrosshairShootingFactor;

}

//����һ�����������ӵ�����ļ�ʱ��
void AShooterCharacter::StartCrosshairBulletFire()
{
	bFiringBullet = true;
	GetWorldTimerManager().SetTimer(
		CrosshairShootTimer,
		this,
		&AShooterCharacter::FinishCrosshairBulletFire,
		ShootTimeDuration);
}

void AShooterCharacter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
}


// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//������׼ʱ���ֵ������
	CameraInterpZoom(DeltaTime);
	//������׼�ı�������
	SetLookRates();
	//����׼�ǵĸ�������
	CalculateCrosshairSpread(DeltaTime);
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
	PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::LookUp);

	//��jump�������ڶ���ӳ��
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this,  &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//������������ڶ���ӳ��
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireWeapon);

	//��AimingButton�������ڶ���ӳ��
	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);


}

//׼�ǵĸ�������getter
float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}




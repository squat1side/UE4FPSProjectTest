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

	//初始化基础速率
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	//Aiming的转向速率
	HipTurnRate(90.f),
	HipLookUpRate(90.f),
	AimingTurnRate(90.f),
	AimingLookUpRate(20.f),
	//鼠标灵敏度瞄准/不瞄准
	MouseHipTurnRate(1.0f),
	MouseHipLookUpRate(1.0f),
	MouseAimingTurnRate(0.2f),
	MouseAimingLookUpRate(0.2f),
	//初始化瞄准变量
	bAiming(false),
	//初始化默认视野和放大后的视野(度)设置在Beginplay
	CameraDefaultFOV(0.f),
	CameraZoomedFOV(35.f),
	//初始化缩放速度
	CameraCurrentFOV(0.f),
	ZoomInterpSpeed(20.f),
	//准星浮动
	CrosshairSpreadMultiplier(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimFactor(0.f),
	CrosshairShootingFactor(0.f),
	//子弹发射计时器变量
	ShootTimeDuration(0.05f),
	bFiringBullet(false)


{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//实现将摄像机杆设置为默认子类组件，并附加在根组件,并设置臂长，设置摄像机跟随视角
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 180.f;
	CameraBoom->bUsePawnControlRotation = true;
	//设置插槽偏移 - 使角色不在屏幕中间  -使摄像机组件设置一些偏移量
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);



	//创建CameraFollow
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//摄像机附加在弹簧臂的末端插槽
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//摄像机跟随吊杆旋转
	FollowCamera->bUsePawnControlRotation = false;

	//控制器只影响摄像机，不影响角色的转动
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	//设置角色的移动
	GetCharacterMovement()->bOrientRotationToMovement = false;//角色在输入方向上移动
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);//旋转速度
	//设置角色跳跃
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//构建跟随视野摄像机
	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
}

//实现MoveForward
void AShooterCharacter::MoveForward(float Value)
{
	//检查继承的控制器的变量是否有效
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		//找出那个方向是前进{初始化}
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		//添加移动输入 -移动角色和移动组件中的其他信息确定
		AddMovementInput(Direction, Value);
	}
}

//实现MoveRight
void AShooterCharacter::MoveRight(float Value)
{
	//检查继承的控制器的变量是否有效
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		//找出那个方向是左右转向{初始化}
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		//添加移动输入 -移动角色和移动组件中的其他信息确定
		AddMovementInput(Direction, Value);
	}
}

//实现视角转动来影响控制器
void AShooterCharacter::TurnAtRate(float Rate)
{
	//根据速率信息计算每一帧的Delta
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


//基于鼠标X/Y移动的旋转控制器的参数值 - 参数值来自鼠标移动的输入值
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

//创建武器开火功能：
void AShooterCharacter::FireWeapon()
{
	//判断开火音效是否有效
	if (FireSound)
	{
		//调用play sound 2D -静态函数 - 游戏的play是静态的
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
	//获取插槽 - 创建一个局部变量
	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());
		//生成粒子特效
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		//创建一个F向量命名为光束
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
			//产生冲击粒子后，处理烟雾粒子
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

	//创建一个局部变量动画实例 - 用于控制蒙太奇中开火动画
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
	//为准星何时开始开火射击子弹
	StartCrosshairBulletFire();

}

//处理获取光束和位置的GetBeamEndLocation函数：
bool AShooterCharacter::GetBeamEndLocation(
	const FVector& MuzzleSocketLocation, 
	FVector& OutBeamLocation)
{
	//获取屏幕中心的准星位置 - 以准星来进行屏幕位置来进行射线追踪
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	//获取屏幕准星位置，创建一个F向量
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	//CrosshairLocation.Y -= 50.f;
	//为准星的世界位置和世界方向创建局部变量
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	//获取世界位置和准星方向
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	//判断bScreenToWorld - 投影是否成功
	if (bScreenToWorld)
	{
		FHitResult ScreenTraceHit;
		const FVector Start{ CrosshairWorldPosition };
		const FVector End(CrosshairWorldPosition + CrosshairWorldDirection * 50000.f);

		//光束追踪
		OutBeamLocation = End;

		//准星射线发射追踪世界位置
		GetWorld()->LineTraceSingleByChannel(
			ScreenTraceHit,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);
		//检查光束命中结果
		if (ScreenTraceHit.bBlockingHit)
		{
			//光束指向命中位置
			OutBeamLocation = ScreenTraceHit.Location;

		}
		//检查从枪口到击中点的轨迹 -枪口与光束点之间存在物体，需要改变子弹撞击点，使其撞击在该物体上
		FHitResult WeaponTraceHit;
		const FVector WeaponTraceStart{ MuzzleSocketLocation };
		const FVector WeaponTraceEnd{ OutBeamLocation };
		GetWorld()->LineTraceSingleByChannel(
			WeaponTraceHit,
			WeaponTraceStart,
			WeaponTraceEnd,
			ECollisionChannel::ECC_Visibility);
		//检查是否命中在击中点和枪口之间的对象
		if (WeaponTraceHit.bBlockingHit)
		{
			OutBeamLocation = WeaponTraceHit.Location;
		}
		return true;
	}

	return false;
}

//为瞄准创建键位变量（按下或松开）
void AShooterCharacter::AimingButtonPressed()
{
	bAiming = true;
}

void AShooterCharacter::AimingButtonReleased()
{
	bAiming = false;
}

//进行重构和简化Tick函数中插值视野代码
void AShooterCharacter::CameraInterpZoom(float DeltaTime)
{
	//检查是否在瞄准 -设置当前摄像机的视野
	if (bAiming)
	{
		//插值到ZoomedFOV
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraZoomedFOV,
			DeltaTime,
			ZoomInterpSpeed);
	}
	else
	{
		//插值到DefaultFOV
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
	//更新Aiming的旋转速率
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

//实现准星浮动的每帧计算
void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange{ 0.f, 600.f };
	FVector2D VelocityMultiplierRange{ 0.f, 1.f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f;

	//计算准星的浮动速度
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(
		WalkSpeedRange,
		VelocityMultiplierRange,
		Velocity.Size());

	//进行检查是否在空中  - 准星浮动在空中
	if (GetCharacterMovement()->IsFalling())
	{
		//在空中时候准星的扩散
		CrosshairInAirFactor = FMath::FInterpTo(
			CrosshairInAirFactor,
			2.25f,
			DeltaTime,
			2.25f);
	}
	else//当角色在地面上
	{
		//在地面上快速缩小准星
		CrosshairInAirFactor = FMath::FInterpTo(
			CrosshairInAirFactor,
			0.f,
			DeltaTime,
			30.f);
	}

	//瞄准因素时候的准星的浮动
	if (bAiming)
	{
		//在短时内快速缩小准星
		CrosshairAimFactor = FMath::FInterpTo(
			CrosshairAimFactor,
			0.6f,
			DeltaTime,
			30.f);
	}
	else
	{
		//准星快速回复正常
		CrosshairAimFactor = FMath::FInterpTo(
			CrosshairAimFactor,
			0.f,
			DeltaTime,
			30.f);
	}

	//在0.05s射击
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


	//准星浮动的计算
	CrosshairSpreadMultiplier = 
		0.5f + 
		CrosshairVelocityFactor + 
		CrosshairInAirFactor - 
		CrosshairAimFactor +
		CrosshairShootingFactor;

}

//创建一个函数启动子弹开火的计时器
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

	//处理当瞄准时候插值到缩放
	CameraInterpZoom(DeltaTime);
	//根据瞄准改变灵敏度
	SetLookRates();
	//计算准星的浮动速速
	CalculateCrosshairSpread(DeltaTime);
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//使用check宏检查输入组件是否有效
	check(PlayerInputComponent);

	//将移动函数绑定到轴映射
	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);

	//将转向速率函数绑定到轴映射
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);

	//将鼠标转向速率函数绑定到轴映射
	PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::LookUp);

	//将jump函数绑定在动作映射
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this,  &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//将武器开火绑定在动作映射
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireWeapon);

	//将AimingButton函数绑定在动作映射
	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);


}

//准星的浮动创建getter
float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}




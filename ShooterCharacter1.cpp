// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"


// Sets default values
AShooterCharacter::AShooterCharacter():

	//初始化基础速率
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f)

{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//实现将摄像机杆设置为默认子类组件，并附加在根组件,并设置臂长，设置摄像机跟随视角
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;

	//创建CameraFollow
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//摄像机附加在弹簧臂的末端插槽
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//摄像机跟随吊杆旋转
	FollowCamera->bUsePawnControlRotation = false;

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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


// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	//将jump函数绑定在动作映射
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this,  &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
}



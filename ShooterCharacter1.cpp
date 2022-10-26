// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"


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

	//����CameraFollow
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//����������ڵ��ɱ۵�ĩ�˲��
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//��������������ת
	FollowCamera->bUsePawnControlRotation = false;

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
}



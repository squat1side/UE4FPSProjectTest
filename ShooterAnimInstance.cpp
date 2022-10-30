// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}
	if (ShooterCharacter)
	{
		//��velocity�л�ȡ��ɫ�����ٶ�
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		Velocity.Z = 0;
		Speed = Velocity.Size();

		//��ɫ�Ƿ��ڿ���
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();
		//��ɫ�Ƿ��ڼ��� - �����ٶ�
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}

		//����һ����ת�� - ��������׼�ķ������Ӧ����ת��
		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		//�ֲ���ת��
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());
		//ʹ��MovementOffset���������������ת��֮��Ĳ���
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(
			MovementRotation,
			AimRotation).Yaw;

		//һ��ֹͣ�ƶ����ͱ����������ƶ�ƫ����
		if (ShooterCharacter->GetVelocity().Size() > 0.f)
		{
			//�������˶�ƫ���������ڡ��˶�ƫ������
			LastMovementOffSetYaw = MovementOffsetYaw;
		}
	}
}


void UShooterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}
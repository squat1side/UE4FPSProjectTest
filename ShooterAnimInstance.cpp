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
		//从velocity中获取角色横向速度
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		Velocity.Z = 0;
		Speed = Velocity.Size();

		//角色是否在空中
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();
		//角色是否在加速 - 检查加速度
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}

		//设置一个旋转器 - 返回与瞄准的方向相对应的旋转器
		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		//局部旋转器
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());
		//使用MovementOffset变量来获得两个旋转器之间的差异
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(
			MovementRotation,
			AimRotation).Yaw;

		//一旦停止移动，就保存了最后的移动偏移量
		if (ShooterCharacter->GetVelocity().Size() > 0.f)
		{
			//“最后的运动偏移量”等于“运动偏移量”
			LastMovementOffSetYaw = MovementOffsetYaw;
		}
	}
}


void UShooterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}
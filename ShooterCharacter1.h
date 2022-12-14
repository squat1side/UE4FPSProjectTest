// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//向前和向后轴映射输入
	void MoveForward(float Value);

	//向左向右的轴映射输入
	void MoveRight(float Value);

	//给定的速率来转动
	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	//将摄像机吊杆设置在角色与摄像机中间
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	//设置摄像机跟随角色
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	//基础转向速率 - 度/秒，其他缩放会影响最终旋转速率
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	//基础上下速率 - 度/秒，其他缩放会影响最终旋转速率
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;


public:
	//返回一个摄像机吊杆子对象 -设置CameraBoom的Getter
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	//返回一个新的摄像机组件指针，将其称为获取跟随相机 -设置Camera的getter
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

};

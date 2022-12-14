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

	//创建武器开火功能：
	void FireWeapon();

	//创建一个函数来处理获取光束和位置
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

	//为瞄准创建键位变量（按下或松开）
	void AimingButtonPressed();
	void AimingButtonReleased();

	//避免在轮流变焦调用摄像机的功能 - 进行重构和简化Tick函数中插值视野代码
	void CameraInterpZoom(float DeltaTime);


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

	//添加fire weapon的声效变量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	//添加开火枪口粒子系统变量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	//控制蒙太奇中开火动画
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	//冲击粒子在子弹射击过程的撞击点
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	//添加一个光束粒子的烟雾轨迹粒子系统
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	//添加一个布尔值用于跟踪何时瞄准和不瞄准
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	//摄像机视野创建一个私有变量
	float CameraDefaultFOV;
	//放大时的视野值
	float CameraZoomedFOV;

	//当前视角框架的范围
	float CameraCurrentFOV;
	//缩放速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;


public:
	//返回一个摄像机吊杆子对象 -设置CameraBoom的Getter
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	//返回一个新的摄像机组件指针，将其称为获取跟随相机 -设置Camera的getter
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//为瞄准的布尔变量创建一个getter
	FORCEINLINE bool GetAiming() const { return bAiming; }


};

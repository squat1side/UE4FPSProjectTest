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

	//��ǰ�������ӳ������
	void MoveForward(float Value);

	//�������ҵ���ӳ������
	void MoveRight(float Value);

	//������������ת��
	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	//�������X�ƶ�����ת�������Ĳ���ֵ - ����ֵ��������ƶ�������ֵ
	void Turn(float Value);
	//�������Y�ƶ�����ת�������Ĳ���ֵ - ����ֵ��������ƶ�������ֵ
	void LookUp(float Value);


	//�������������ܣ�
	void FireWeapon();

	//����һ�������������ȡ������λ��
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

	//Ϊ��׼������λ���������»��ɿ���
	void AimingButtonPressed();
	void AimingButtonReleased();

	//�����������佹����������Ĺ��� - �����ع��ͼ�Tick�����в�ֵ��Ұ����
	void CameraInterpZoom(float DeltaTime);
	//������Ϸ��������׼�Ļ��������û�����ת�ʺͻ���Lookuprate
	void SetLookRates();

	//ʵ��׼�Ǹ�����ÿ֡����
	void CalculateCrosshairSpread(float DeltaTime);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	//����������������ڽ�ɫ��������м�
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	//��������������ɫ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	//����ת������ - ��/�룬�������Ż�Ӱ��������ת����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	//������������ - ��/�룬�������Ż�Ӱ��������ת����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	//���Hip��׼ʱ�ı��� - ����׼ʱ
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipTurnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipLookUpRate;
	//�����׼ʱת���Lookup
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingTurnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingLookUpRate;

	//��������ص�Hip��Aiming��ת����� -��������ȵı��� - ��������׼ʱ
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookUpRate;
	//��������ȵı��� - ������׼ʱ
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingLookUpRate;

	//���fire weapon����Ч����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	//��ӿ���ǹ������ϵͳ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	//������̫���п��𶯻�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	//����������ӵ�������̵�ײ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	//���һ���������ӵ�����켣����ϵͳ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	//���һ������ֵ���ڸ��ٺ�ʱ��׼�Ͳ���׼
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	//�������Ұ����һ��˽�б���
	float CameraDefaultFOV;
	//�Ŵ�ʱ����Ұֵ
	float CameraZoomedFOV;

	//��ǰ�ӽǿ�ܵķ�Χ
	float CameraCurrentFOV;
	//�����ٶ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	//����׼�Ǹ�������
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;
	//���ø����� - ����׼�Ǹ��� - ׼�Ǹ������ٶ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;
	//׼�Ǹ����Ŀ�������
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;
	//��׼ʱ׼��������
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor;
	//���ʱ׼�Ǹ�������
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;



public:
	//����һ������������Ӷ��� -����CameraBoom��Getter
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	//����һ���µ���������ָ�룬�����Ϊ��ȡ������� -����Camera��getter
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//Ϊ��׼�Ĳ�����������һ��getter
	FORCEINLINE bool GetAiming() const { return bAiming; }


};

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


public:
	//����һ������������Ӷ��� -����CameraBoom��Getter
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	//����һ���µ���������ָ�룬�����Ϊ��ȡ������� -����Camera��getter
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

};
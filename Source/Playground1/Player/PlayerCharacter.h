// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class PLAYGROUND1_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// Player camera
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

protected:
	// Input
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* InputMap;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* CameraMovementAction;

protected:
	void Movement(const FInputActionValue& InputValue);
	void Look(const FInputActionValue& InputValue);
	void Jump();

};

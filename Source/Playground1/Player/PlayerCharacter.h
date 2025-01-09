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

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma region Health_Functions
	// Return player's health.
	UFUNCTION(BlueprintPure, Category= "Player Health")
	int GetHealth();

	// Return the player's max health.
	UFUNCTION(BlueprintPure, Category = "Player Health")
	int GetMaxHealth();

	// Increase/Decrease player's health.
	UFUNCTION(BlueprintCallable, Category = "Player Health")
	void UpdateHealth(int Health);

	// Set the player's max health.
	UFUNCTION(BlueprintCallable, Category = "Player Health")
	void SetMaxHealth(int Health);
#pragma endregion

#pragma region Stamina_Functions

	// Return current stamina.
	UFUNCTION(BlueprintPure, Category = "Player Stamina")
	float GetStamina();

	// Set player's stamina.
	UFUNCTION(BlueprintCallable, Category = "Player Stamina")
	void SetStamina(float Stamina);

	// Set player's stamina recovery amount.
	UFUNCTION(BlueprintCallable, Category = "Player Stamina")
	void SetStaminaRecoveryValue(float Recovery);
#pragma endregion




protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#pragma region Inputs

	// Input
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* InputMap;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* CameraMovementAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* KickAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* SprintAction;

#pragma endregion

	// Player Actions
	void Movement(const FInputActionValue& InputValue);
	void Look(const FInputActionValue& InputValue);
	void Jump();
	void Crouch();
	void Kick();
	void Sprint();

private:
	// Camera
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	// Health
	static constexpr int DefaultHealth  = 100;
	int					 CurrentHealth  = DefaultHealth;
	int					 MaxHealth	    = DefaultHealth;

	// Speed
	static constexpr float WalkSpeed   = 600.f;
	static constexpr float SprintSpeed = WalkSpeed * 2.f;
	bool IsRunning;

	// Stamina
	float				   CurrentStamina  = 100.f;
	float				   StaminaRecovery = 5.f;
	static float constexpr MaxStamina	   = 100.f;


	static constexpr float SprintCost	   = 5.f;
	static constexpr float JumpCost		   = 10.f;
	static constexpr float KickCost		   = 15.f;






};

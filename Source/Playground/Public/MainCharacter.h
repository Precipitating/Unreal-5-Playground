// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MainCharacter.generated.h"

UCLASS()
class PLAYGROUND_API AMainCharacter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMainCharacter();

	void MoveSideways(float delta);
	void MoveForwardsBackwards(float delta);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Player camera
	UPROPERTY(EditAnywhere)
	UCameraComponent* camera;

	// Player mesh
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* cameraMesh;

	// Character settings
	UPROPERTY(EditAnywhere, Category = "Character Settings:")
	float playerSpeed = 1.f;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

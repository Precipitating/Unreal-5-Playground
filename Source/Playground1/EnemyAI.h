// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyAI.generated.h"

UCLASS()
class PLAYGROUND1_API AEnemyAI : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyAI();

#pragma region Ragdoll
	UFUNCTION(BlueprintCallable, Category = "Ragdoll Simulation")
	void StartRagdoll();
	UFUNCTION(BlueprintCallable, Category = "Ragdoll Simulation")
	void StopRagdoll();
	UFUNCTION(BlueprintCallable, Category = "Ragdoll Simulation")
	void EndRagdoll();
	UFUNCTION(BlueprintCallable, Category = "Ragdoll Simulation")
	void UpdateRagdoll();

	UFUNCTION(BlueprintPure, Category = "Ragdoll Getters")
	bool GetIsRagdoll();
	UFUNCTION(BlueprintPure, Category = "Ragdoll Getters")
	FVector GetRagdollVelocity();
#pragma endregion



private:
	USkeletalMeshComponent* SkeletalMesh = nullptr;
	FName RootBone = "pelvis";

	// Stand up animation
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* StandUpAnimation;

	// Timer handle to manage the timer
	FTimerHandle TimerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnemyAI.generated.h"

class UCurveFloat;
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
	void EndRagdoll();
	UFUNCTION(BlueprintCallable, Category = "Ragdoll Simulation")
	void UpdateRagdoll();

	UFUNCTION(BlueprintCallable, Category = "Ragdoll Simulation")
	void CharacterStanding();

	UFUNCTION(BlueprintPure, Category = "Ragdoll Getters")
	bool GetIsRagdoll();

	UFUNCTION(BlueprintCallable, Category = "Ragdoll Getters")
	void GetIsRagdollFacingDown();


#pragma endregion



private:
	bool IsRagdoll = false;
	bool IsFallBack = false;
	USkeletalMeshComponent* SkeletalMesh = nullptr;
	FName RootBone = "pelvis";

	// Stand up animation
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* StandUpAnimation;

	// Stand up facing down
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* StandUpDownAnimation;

	FTimeline StandUpTimeline;

	// Timer
	FTimerHandle TimerHandle;



private:
	void StandUpUpdate(float Alpha);
	void StandUpFinished();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Timer handle to manage the timer
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stand up")
	UCurveFloat* StandUpCurve = nullptr;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

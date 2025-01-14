// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAI.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

// Sets default values
AEnemyAI::AEnemyAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SkeletalMesh = GetMesh();

}

#pragma region Ragdoll
void AEnemyAI::StartRagdoll()
{
	if (!IsRagdoll)
	{
		IsRagdoll = true;
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SkeletalMesh->SetAllBodiesBelowSimulatePhysics(RootBone, true);
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemyAI::GetIsRagdollFacingDown, 0.5f, true);
	}


}

void AEnemyAI::EndRagdoll()
{
	if (IsRagdoll)
	{
		IsRagdoll = false;
		SkeletalMesh->AttachToComponent(GetCapsuleComponent(), 
			FAttachmentTransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
				true));
		SkeletalMesh->SetRelativeLocationAndRotation(FVector(0.f, 0.f, 80.f), FRotator(0.f, 0.f, -90.f));
		SkeletalMesh->SetSimulatePhysics(false);

		if (IsFallBack)
		{
			PlayAnimMontage(StandUpDownAnimation);
		}
		else
		{
			FRotator NewZ = SkeletalMesh->GetSocketRotation("pelvis") + FRotator(0, 0, 180);
			GetCapsuleComponent()->SetWorldRotation(NewZ);
			PlayAnimMontage(StandUpAnimation);
		}

		StandUpTimeline.ReverseFromEnd();
	}


}
void AEnemyAI::UpdateRagdoll()
{

}
void AEnemyAI::CharacterStanding()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

}
bool AEnemyAI::GetIsRagdoll()
{
	return IsRagdoll;

}

void AEnemyAI::GetIsRagdollFacingDown()
{
	// Get raycast to floor so we dont clip through ground
	FRotator Start = SkeletalMesh->GetSocketRotation("pelvis");
	FVector RightVec = UKismetMathLibrary::GetRightVector(Start);
	double DotProd = UKismetMathLibrary::Dot_VectorVector(RightVec, FVector(0, 0, 1));
	bool StoppedMoving = UKismetMathLibrary::Vector_IsNearlyZero(SkeletalMesh->GetPhysicsLinearVelocity(), 5.f);

	if (StoppedMoving)
	{
		// Character facing up
		if (DotProd > 0)
		{
			IsFallBack = false;
		}
		else
		{
			IsFallBack = true;
		}

		TimerHandle.Invalidate();

		EndRagdoll();
	}


}


void AEnemyAI::StandUpUpdate(float Alpha)
{
	SkeletalMesh->SetAllBodiesBelowPhysicsBlendWeight(FName("pelvis"), Alpha);

}

void AEnemyAI::StandUpFinished()
{
	SkeletalMesh->SetSimulatePhysics(false);
}
#pragma endregion



// Called when the game starts or when spawned
void AEnemyAI::BeginPlay()
{
	Super::BeginPlay();

	FOnTimelineFloat StandUpdate;
	StandUpdate.BindUFunction(this, FName("StandUpUpdate"));

	FOnTimelineEvent FinishedStandUp;
	StandUpdate.BindUFunction(this, FName("StandUpFinished"));

	StandUpTimeline.AddInterpFloat(StandUpCurve, StandUpdate);
	StandUpTimeline.SetTimelineFinishedFunc(FinishedStandUp);
}

void AEnemyAI::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

}


// Called every frame
void AEnemyAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetIsRagdoll())
	{
		// get raycast to floor so we dont clip through ground
		FVector Start = SkeletalMesh->GetSocketLocation("pelvis");
		FVector End = SkeletalMesh->GetSocketLocation("pelvis") + (FVector(0, 0, -1.f) * 90.f);
		FHitResult Out;
		bool result =  GetWorld()->LineTraceSingleByChannel(Out, Start, End, ECollisionChannel::ECC_Visibility);

		if (result)
		{
			GetCapsuleComponent()->SetWorldLocation(Out.Location + FVector(0, 0, 90.f));
		}
		else
		{
			GetCapsuleComponent()->SetWorldLocation(Start);
		}


	}

}

// Called to bind functionality to input
void AEnemyAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


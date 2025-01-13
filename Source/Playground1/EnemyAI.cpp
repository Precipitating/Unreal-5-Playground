// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAI.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	// Disable collision
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// Disable all collision and movement 
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	SkeletalMesh->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	SkeletalMesh->SetAllBodiesBelowSimulatePhysics(RootBone, true);

	UpdateRagdoll();

}
void AEnemyAI::StopRagdoll()
{
	// Reverse everything from StartRagdoll
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	SkeletalMesh->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	SkeletalMesh->SetAllBodiesBelowSimulatePhysics(RootBone, false);

	SkeletalMesh->GetAnimInstance()->SavePoseSnapshot("Ragdoll");
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemyAI::StopRagdoll, SkeletalMesh->GetAnimInstance()->Montage_Play(StandUpAnimation), false);
	
}
void AEnemyAI::EndRagdoll()
{
	// allow movement 
}
void AEnemyAI::UpdateRagdoll()
{
	FTransform SocketTransform = SkeletalMesh->GetSocketTransform(RootBone);
	FRotator AdjustedRotation = GetActorRotation();
	AdjustedRotation.Roll = SocketTransform.GetRotation().Z;

	if (AdjustedRotation.Roll < 0)
	{
		AdjustedRotation.Roll -= 180.f;
	}

	SetActorRotation(AdjustedRotation);
	SetActorLocation(SocketTransform.GetLocation() + FVector(0, 0, 90));


	// Set ragdoll stiffness
	double RagdollVelocityLength = GetRagdollVelocity().Length();
	float Spring = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 500.f), FVector2D(500.f, 5000.f), RagdollVelocityLength);

	SkeletalMesh->SetAllMotorsAngularDriveParams(Spring, 0.f, 0.f);


	
}
bool AEnemyAI::GetIsRagdoll()
{
	return IsValid(SkeletalMesh) ? SkeletalMesh->IsSimulatingPhysics(RootBone) : false;

}

FVector AEnemyAI::GetRagdollVelocity()
{

	return IsValid(SkeletalMesh) ? SkeletalMesh->GetBoneLinearVelocity(RootBone) : FVector();

}
#pragma endregion

// Called when the game starts or when spawned
void AEnemyAI::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetIsRagdoll())
	{
		UpdateRagdoll();
	}

}

// Called to bind functionality to input
void AEnemyAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


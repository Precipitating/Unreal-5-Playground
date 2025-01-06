// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	cameraMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraMesh"));
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	camera->SetupAttachment(cameraMesh);
	camera->FieldOfView = 103.f;


}

void AMainCharacter::MoveSideways(float delta)
{
	FVector currPos = GetActorLocation();

	currPos.Y += delta * playerSpeed;
	SetActorLocation(currPos);
}

void AMainCharacter::MoveForwardsBackwards(float delta)
{
	FVector currPos = GetActorLocation();

	currPos.X += delta * playerSpeed;
	SetActorLocation(currPos);
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Register movement
	PlayerInputComponent->BindAxis(TEXT("MoveLR"), this, &AMainCharacter::MoveSideways);
	PlayerInputComponent->BindAxis(TEXT("MoveFB"), this, &AMainCharacter::MoveForwardsBackwards);

}


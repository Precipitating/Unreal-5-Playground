#include "Playground1/Player/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//SetActorTickInterval(0.5f);
	//SetActorTickEnabled(true);
	SetTickGroup(ETickingGroup::TG_PostUpdateWork);
	
	// Setup spring arm.
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetMesh(), FName("head"));
	SpringArm->TargetArmLength = 0.f;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = false;

	// Setup camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->SetWorldLocation(CameraLocation);
	Camera->bUsePawnControlRotation = true;


}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// enable crouching
	if (GetMovementComponent())
	{
		GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	}

	//LegCollider->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::BeginKickOverlap);

	Mesh = GetMesh();
	KickEvent = this->FindFunction(FName("Kick"));

	GetWorld()->GetTimerManager().SetTimer(StaminaTimerHandle, this, &APlayerCharacter::RegenStamina, StaminaRegenDelay, true);
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Ensure the fuze timer is cleared by using the timer handle
	GetWorld()->GetTimerManager().ClearTimer(StaminaTimerHandle);

	// Alternatively you can clear ALL timers that belong to this (Actor) instance.
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float PreviousStamina = CurrentStamina;

	if (HasJumped)
	{
		CurrentStamina -= JumpCost;

	}
	else if (HasRan)
	{
		CurrentStamina -= SprintCost * DeltaTime;
	}

	if (CurrentStamina != PreviousStamina)
	{
		OnStaminaUpdate.Broadcast(PreviousStamina, CurrentStamina, MaxStamina);
	}

	HasRan = false;
	HasJumped = false;

	// Debug
	//GEngine->AddOnScreenDebugMessage(-1, 0.49f, FColor::Silver,
	//	*(FString::Printf(
	//		TEXT("Movement - IsCrouched:%d | IsSprinting:%d"), bIsCrouched, IsRunning)));
	//GEngine->AddOnScreenDebugMessage(-1, 0.49f, FColor::Green,
	//	*(FString::Printf(
	//		TEXT("Stamina - Current:%f | Maximum:%f"), CurrentStamina, MaxStamina)));




}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Add input mapping context
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	checkf(PlayerController, TEXT("Unable to get PlayerController reference."));

	// Get local player subsystem
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	checkf(Subsystem, TEXT("Unable to get Subsystem reference."));
	
	// Add input context
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMap, 0);



	// Bind actions to functions
	UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	checkf(Input, TEXT("Unable to get Input reference."));
	Input->BindAction(MovementAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Movement);
	Input->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Jump);
	Input->BindAction(CameraMovementAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
	Input->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Crouch);
	Input->BindAction(KickAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Kick);
	Input->BindAction(SprintAction, ETriggerEvent::Triggered, this, &APlayerCharacter::SetSprint, true);
	Input->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::SetSprint, false);


}


#pragma region Stamina_Functions
float APlayerCharacter::GetStamina()
{
	return CurrentStamina;
}
void APlayerCharacter::SetStamina(float Stamina)
{
	CurrentStamina = Stamina;
}
void APlayerCharacter::ReduceStamina(float Stamina)
{
	CurrentStamina -= Stamina;
}
void APlayerCharacter::SetStaminaRecoveryValue(float Recovery)
{
	StaminaRecoveryFactor = Recovery;
}
void APlayerCharacter::RegenStamina()
{
	if (bIsCrouched)
	{
		CurrentStamina += CrouchRecovery;
	}
	else
	{
		CurrentStamina += StaminaRecoveryFactor;
	}

	// Ensure no over/undershooting of stamina
	CurrentStamina = FMath::Clamp(CurrentStamina, 0.f, MaxStamina);
}
#pragma endregion


#pragma region Action_Getter_Functions
bool APlayerCharacter::GetJumped() const
{
	return HasJumped || GetCharacterMovement()->IsFalling();
}

bool APlayerCharacter::GetWalking() const 
{
	bool IsMoving = ACharacter::GetVelocity().SizeSquared() <= 0.1f;

	return !IsMoving;
}

bool APlayerCharacter::GetRunning() const 
{
	return IsRunning;
}

bool APlayerCharacter::GetCrouching() const 
{
	return bIsCrouched;
}

bool APlayerCharacter::GetKicked() const 
{
	return HasKicked;
}

void APlayerCharacter::SetKicked(bool Value)
{
	HasKicked = Value;
}


#pragma endregion

void APlayerCharacter::Movement(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();

	if (IsValid(Controller) && CanMove)
	{
		
		// Add Movement
		AddMovementInput(GetActorForwardVector(), InputVector.Y);
		AddMovementInput(GetActorRightVector(), InputVector.X);

		if (IsRunning)
		{
			HasRan = true;
		}


	}
}



void APlayerCharacter::Look(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();

	if (IsValid(Controller))
	{
		AddControllerYawInput(InputVector.X);
		AddControllerPitchInput(InputVector.Y);

	}
}
#pragma region Player_Actions
void APlayerCharacter::Jump()
{
	if (JumpAction && ((CurrentStamina - JumpCost) > 0.f))
	{
		if (CanJump())
		{
			if (bIsCrouched)
			{
				UnCrouch();
			}

			Super::Jump();
			HasJumped = true;
		}
	}

}

void APlayerCharacter::Crouch()
{
	if (CrouchAction && !GetJumped())
	{
		SetSprint(false);
		if (bIsCrouched)
		{
			ACharacter::UnCrouch();
		}
		else
		{
			ACharacter::Crouch();
		}

	}


}

void APlayerCharacter::Kick()
{
	if (KickAction && (CurrentStamina - KickCost) > 0.f && !HasKicked)
	{
		CurrentStamina -= KickCost;
		this->ProcessEvent(KickEvent, nullptr);
		
	}

}

//void APlayerCharacter::BeginKickOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	// Make sure it's not itself.
//	if (OtherActor && OtherActor == this) { return; }
//
//	// Handle kick if it overlaps a capsule component.
//	if (HasKicked)
//	{
//		UFunction* RagdollBeginEvent = OtherActor->FindFunction(TEXT("RagdollBegin"));
//		if (RagdollBeginEvent)
//		{
//			OtherActor->ProcessEvent(RagdollBeginEvent, nullptr);
//		}
//
//
//
//
//
//	}
//
//
//}

void APlayerCharacter::SetSprint(bool IsSprinting)
{
	// Do not sprint if we have no stamina, or we are crouching.
	if ((IsRunning && CurrentStamina <= 0.f) || (CurrentStamina - SprintCost) <= 0.f || GetCrouching())
	{
		IsSprinting = false;
	}

	IsRunning = IsSprinting;

	GetCharacterMovement()->MaxWalkSpeed = IsRunning ? SprintSpeed : WalkSpeed;
}



#pragma endregion



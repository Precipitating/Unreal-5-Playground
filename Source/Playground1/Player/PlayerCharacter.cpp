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
	SetActorTickInterval(0.5f);
	SetActorTickEnabled(true);
	
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

	// Setup Leg Collider
	LegCollider = CreateDefaultSubobject<USphereComponent>(TEXT("LegCollider"));
	LegCollider->SetupAttachment(GetMesh(), FName("ball_l"));
	LegCollider->SetSphereRadius(15.f);
	LegCollider->SetWorldLocation(FVector(0.35f, 6.f, 4.f));
	LegCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

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

	LegCollider->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::BeginKickOverlap);


	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle stamina reductions and regeneration.
	// Adjust regen rate depending on action.
	float CurrentStaminaRegen = StaminaRecoveryFactor;


	if (HasJumped)
	{
		CurrentStaminaRegen = -JumpCost;

	}
	else if (HasRan)
	{
		CurrentStaminaRegen = -SprintCost;
	}
	else if (bIsCrouched)
	{
		CurrentStaminaRegen = CrouchRecovery;
	}

	if (HasKicked)
	{
		CurrentStaminaRegen -= KickCost;

		
	}

	const float PreviousStamina = CurrentStamina;

	// Ensure no over/undershooting of stamina

	CurrentStamina = FMath::Clamp(CurrentStamina + CurrentStaminaRegen, 0.f, MaxStamina);

	if (CurrentStamina != PreviousStamina)
	{
		OnStaminaUpdate.Broadcast(PreviousStamina, CurrentStamina, MaxStamina);
	}

	HasRan = false;
	HasJumped = false;


	// Debug
	GEngine->AddOnScreenDebugMessage(-1, 0.49f, FColor::Silver,
		*(FString::Printf(
			TEXT("Movement - IsCrouched:%d | IsSprinting:%d"), bIsCrouched, IsRunning)));
	GEngine->AddOnScreenDebugMessage(-1, 0.49f, FColor::Red,
		*(FString::Printf(
			TEXT("Health - Current:%d | Maximum:%d"), CurrentHealth, MaxHealth)));
	GEngine->AddOnScreenDebugMessage(-1, 0.49f, FColor::Green,
		*(FString::Printf(
			TEXT("Stamina - Current:%f | Maximum:%f"), CurrentStamina, MaxStamina)));




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

#pragma region Health_Functions
int APlayerCharacter::GetHealth()
{
	return CurrentHealth;
}

int APlayerCharacter::GetMaxHealth()
{
	return MaxHealth;
}

void APlayerCharacter::UpdateHealth(int Health)
{
	int PreviousHealth = CurrentHealth;

	CurrentHealth += Health;

	// Clamp so it doesnt overshoot/undershoot.
	CurrentHealth = FMath::Clamp(CurrentHealth, -1.f, MaxHealth);

	// Notify listeners if there is a change in health.
	if (CurrentHealth != PreviousHealth)
	{
		OnHealthUpdated.Broadcast(PreviousHealth, CurrentHealth, MaxHealth);
	}

	// Handle death, if negating health makes it < 0.
	if (CurrentHealth <= 0)
	{
		OnPlayerDied.Broadcast();
		
	}
}

void APlayerCharacter::SetMaxHealth(int Health)
{
	MaxHealth = Health;
}
#pragma endregion

#pragma region Stamina_Functions
float APlayerCharacter::GetStamina()
{
	return CurrentStamina;
}
void APlayerCharacter::SetStamina(float Stamina)
{
	CurrentStamina = Stamina;
}
void APlayerCharacter::SetStaminaRecoveryValue(float Recovery)
{
	StaminaRecoveryFactor = Recovery;
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

	if (IsValid(Controller))
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
		PlayAnimMontage(KickMontage);
		
	}

}

void APlayerCharacter::BeginKickOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Make sure it's not itself.
	if (OtherActor && OtherActor == this) { return; }

	// Handle kick if it overlaps a capsule component.
	if (HasKicked)
	{
		UFunction* RagdollBeginEvent = OtherActor->FindFunction(TEXT("RagdollBegin"));
		if (RagdollBeginEvent)
		{
			OtherActor->ProcessEvent(RagdollBeginEvent, nullptr);
		}





	}


}

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



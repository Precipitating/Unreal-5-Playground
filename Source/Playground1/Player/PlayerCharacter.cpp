#include "Playground1/Player/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Ticsk() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Camera"));
	Camera->SetupAttachment(RootComponent);
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


	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	Input->BindAction(KickAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Kick);


}

#pragma region Health_Functions
int APlayerCharacter::GetHealth()
{
	return 0;
}

int APlayerCharacter::GetMaxHealth()
{
	return 0;
}

void APlayerCharacter::UpdateHealth(int Health)
{
}

void APlayerCharacter::SetMaxHealth(int Health)
{
}
#pragma endregion

#pragma region Stamina_Functions
float APlayerCharacter::GetStamina()
{
	return 0.0f;
}
void APlayerCharacter::SetStamina(float Stamina)
{
}
void APlayerCharacter::SetStaminaRecoveryValue(float Recovery)
{
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

void APlayerCharacter::Jump()
{
	if (JumpAction)
	{
		ACharacter::Jump();
	}

}

void APlayerCharacter::Crouch()
{
	if (CrouchAction)
	{
		if (ACharacter::bIsCrouched)
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
	if (KickAction)
	{
		// implement kick action
	}

}

void APlayerCharacter::Sprint()
{
	if (SprintAction)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}



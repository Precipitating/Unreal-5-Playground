#include "Playground1/Player/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

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
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Add input context
			Subsystem->AddMappingContext(InputMap, 0);
		}
	}

	// Bind actions to functions
	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(MovementAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Movement);
		Input->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Jump);
		Input->BindAction(CameraMovementAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
	}

}

void APlayerCharacter::Movement(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();

	if (IsValid(Controller))
	{
		// Forward direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator Yaw(0, Rotation.Yaw, 0);

		const FVector ForwardDir = FRotationMatrix(Yaw).GetUnitAxis(EAxis::X);
		const FVector RightDir = FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y);

		// Add Movement
		AddMovementInput(ForwardDir, InputVector.Y);
		AddMovementInput(RightDir, InputVector.X);

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
	ACharacter::Jump();
}



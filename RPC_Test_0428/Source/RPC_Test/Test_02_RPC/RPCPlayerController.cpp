#include "RPCPlayerController.h"
#include "EnhancedInputComponent.h"
#include "RPCPlayerCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

ARPCPlayerController::ARPCPlayerController()
{
}

void ARPCPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ARPCPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (!InputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("InputComponent is null in %s"), *GetName());
		return;
	}
	
	UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!enhancedInputComponent)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("InputComponent is not UEnhancedInputComponent. Check Project Settings > Engine > Input > Default Classes.")
		);		
		return;
	}
	
	// OnMove
	if (MoveAction)
	{
		enhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARPCPlayerController::OnMove);
		enhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Started, this, &ARPCPlayerController::OnClick);
	}
}

void ARPCPlayerController::OnMove(const FInputActionValue& value)
{
	FVector2D input = value.Get<FVector2D>();
	ACharacter* character = GetCharacter();
	
	if (character == nullptr || input.IsNearlyZero())
	{
		return;
	}

	FVector vec = FVector(input.X, input.Y, 0.f);
	character->AddMovementInput(FVector::ForwardVector, vec.X);
	character->AddMovementInput(FVector::RightVector, vec.Y);
}

void ARPCPlayerController::OnClick()
{
	ARPCPlayerCharacter* character = Cast<ARPCPlayerCharacter>(GetCharacter());
	if (character == nullptr)
	{
		return;
	}
	
	UE_LOG(LogTemp, Error, TEXT("OnClick"));	
	
	character->StartFire();
}

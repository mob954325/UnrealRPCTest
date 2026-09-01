#include "BasePlayerController.h"
#include "EnhancedInputSubsystems.h"

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetInputMappingContext();
}

void ABasePlayerController::SetInputMappingContext()
{
	// input mapping context 추가하기
	if (APlayerController* pc = Cast<APlayerController>(this))
	{
		if (ULocalPlayer* lp = pc->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* subSystem =
				lp->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				subSystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}

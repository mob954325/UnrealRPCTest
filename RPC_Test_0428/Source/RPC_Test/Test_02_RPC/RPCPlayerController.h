#pragma once

#include "CoreMinimal.h"
#include "RPC_Test/BasePlayerController.h"
#include "InputActionValue.h"
#include "RPCPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
/**
 * RPCPlayer Character가 사용하는 캐릭터 컨트롤러
 */
UCLASS()
class RPC_TEST_API ARPCPlayerController : public ABasePlayerController
{
	GENERATED_BODY()
public:
	ARPCPlayerController();
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LeftClickAction;	
	
protected:
	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;
	
private:
	void OnMove(const FInputActionValue& value);
	void OnClick();
};

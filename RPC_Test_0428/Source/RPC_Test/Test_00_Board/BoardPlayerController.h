#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "RPC_Test/BasePlayerController.h"
#include "BoardPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

/**
 * BoardPlayerPawn이 사용할 플레이어 컨트롤러로 보드 클릭 관련 내용을 가지고 있다.
 */
UCLASS()
class RPC_TEST_API ABoardPlayerController : public ABasePlayerController
{
	GENERATED_BODY()
public:
	ABoardPlayerController();	
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ClickAction;
	
	virtual void Tick(float DeltaSeconds) override;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;
	
private:
	
	// 클릭 인풋 액션 내용
	void OnClick(const FInputActionValue& value);
	
	// 움직여하는지 플래그 함수
	bool bShouldMove = false;
	
	// Spline 컴포넌트를 통해 움직이는 함수 (Tick에서 호출)
	void MoveBySpline();
	
	float currnetDistance = -1.f;
	float goalDistance = -1.f;
	FVector goalLocation;
	float dirValue = 1.f;
};

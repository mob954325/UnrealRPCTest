#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RPC_Test/BasePlayerController.h"
#include "InputActionValue.h"
#include "BoardRPCPlayerController.generated.h"

class UBoardRPCWidgetBase;
class UInputMappingContext;
class UInputAction;

/**
 * BoardPlayerPawn이 사용할 플레이어 컨트롤러로 보드 클릭 관련 내용을 가지고 있다.
 * 
 * 플레이어 클릭 이벤트를 받고 움직임을 서버에 요청한다.
 */
UCLASS()
class RPC_TEST_API ABoardRPCPlayerController : public ABasePlayerController
{
	GENERATED_BODY()
public:
	ABoardRPCPlayerController();	
	
	virtual void Tick(float DeltaSeconds) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ClickAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UBoardRPCWidgetBase> BoardRPCWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TObjectPtr<UBoardRPCWidgetBase> BoardRPCWidgetInstance;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;
	
private:
	
	// 클릭 인풋 액션 내용
	void OnClick(const FInputActionValue& value);
	
	// 클릭 시 Pawn에게 위치 이동한다고 요청하는 rpc 함수
	UFUNCTION(Server, Reliable)
	void RequestMove(const int index);
	
	// 움직일 수 있는 위치인지 확인
	bool IsValidMove(const int targetIndex);
};

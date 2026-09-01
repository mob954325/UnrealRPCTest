#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

class UInputMappingContext;

/**
 * 해당 언리엎 프로젝트에서 모든 PlayerController가 상속받는 클래스로
 * 
 * 향상된 인풋 시스템 세팅 내용만 있음
 * 해당 내용은 BeginPlay()에서 호출됨.
 */
UCLASS()
class RPC_TEST_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;
	
protected:
	virtual void BeginPlay() override;
	
	void SetInputMappingContext();
};

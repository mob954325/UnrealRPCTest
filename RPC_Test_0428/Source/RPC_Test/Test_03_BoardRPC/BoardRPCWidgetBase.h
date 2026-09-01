// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BoardRPCWidgetBase.generated.h"

class UTextBlock;
/**
 * BoardRPCPlayerController가 가지고 있는 보드 정보 UI 위젯 베이스 클래스
 * 
 * 현재 남은 시간, 현재 라운드 수를 텍스트로 표시한다.
 */
UCLASS()
class RPC_TEST_API UBoardRPCWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeOnInitialized() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> LeftBoardTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentRound;
	
	UFUNCTION(BlueprintCallable)
	void UpdateLeftBoardTime(int32 currTime);
	
	UFUNCTION(BlueprintCallable)
	void UpdateCurrnetRound(int32 currRound);
};

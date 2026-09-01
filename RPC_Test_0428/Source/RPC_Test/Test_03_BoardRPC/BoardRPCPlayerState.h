#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BoardRPCPlayerState.generated.h"

/**
 * 모든 보드 플레이어가 가지고 있는 PlayerState
 * 
 * 현재 위치의 보드 인덱스를 가지고 있음
 * 보드에서 필요한 각 플레이어의 규칙을 가지고 있다.
 */
UCLASS()
class RPC_TEST_API ABoardRPCPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	int GetCurrentIndex() const;
	void SetCurrentIndex(int InIndex);
	
	void DecreaseMoveCount(int value);
	int GetRemainMoveCount() const;

protected:	
	// 움직입 횟수 초기화 함수	
	void ResetMoveCount();
	
	// 델리게이트 추가를 위한 ResetMoveCount 래핑함수
	UFUNCTION()
	void ResetMovecountDelegateWrapper(int32 currentRound);
	
private:
	// 현재 플레이어가 있는 인덱스 값
	UPROPERTY(ReplicatedUsing = OnRep_CurrentIndex)
	int currentIndex = 0;
	
	UFUNCTION()
	void OnRep_CurrentIndex();
	
	// 최대 움직일 수 있는 횟수
	int maxMoveCount = 1; // note : 이게 변동될 여지가 있으면 이것도 리플리케이트하기.
	
	// 현재 턴에서 움직일 수 있는 횟수
	UPROPERTY(ReplicatedUsing = OnRep_RemainMoveCount)
	int remainMoveCount = 0;
	
	UFUNCTION()	
	void OnRep_RemainMoveCount();
};

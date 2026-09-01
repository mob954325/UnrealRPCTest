#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BoardRPCGameState.generated.h"

class ABoardRPCPlayerPawn;

// 라운드 변경 시 호출되는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeBoardRound, int32, CurrentRound);

/**
 * 보드 상태를 가지고 있는 GameState
 * 현재 진행 턴 수, 자기장 상황 등 관리 
 * 
 * 1. 공동 타이머가 돌아간다.
 * 2. 타이머 종료되면 노드 정보 갱신
 * 3. 1-2 번 반복
 * 4. 라운드 수(CurrentRound)가 최대 라운드 수(MaxRound)까지 도달하면 게임은 끝난다.
 */
UCLASS()
class RPC_TEST_API ABoardRPCGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	// 함수
		// 턴 시작
		// 턴 종료 확인
		// 플레이어 선택 확정 ( 클릭 시 호출해서 해당 플레이어는 움직임을 알림 )
	// 변수
		// 현재 턴 수
		// 플레이어 선택 상황 ( 움직임 여부 -> 턴 종료 확인 하기 위해서 )
		// 보드의 상황, 생성된 boardComponent를 가져온다. ( 이건 자기장, 보드 이벤트 등등 )
	
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintCallable)
	void StartTrun();
	
	UFUNCTION(BlueprintCallable)
	void EndTurn();
	
private:
	// note : TArray<TObjectPtr<APlayerState>> PlayerArray; 를 가지고 있어서 이거 사용하면 될듯
	// TArray<ABoardRPCPlayerPawn*> playerPawns;

	// 보드 타이머
	// 서버에서 갱신하고 클라이언트에는 뷰만 제공
	// 갱신은 GM이 담당한다.	 ( 보드의 규칙 )
	// 값은 GameState에 둔다. ( 모든 플레이어가 알아야할 값 )
	UPROPERTY(ReplicatedUsing = OnRep_LeftBoardTime)
	int32 LeftBoardTime = 0;
	
	UPROPERTY()	
	int32 MaxBoardTime = 5;
	
	UFUNCTION()
	void OnRep_LeftBoardTime();	
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentRound)
	int32 CurrentRound = 0;
	
	UFUNCTION()
	void OnRep_CurrentRound();	
	
	UPROPERTY()
	int32 MaxRound = 10;
	
public:
	void SetLeftBoardTime(int32 value);
	int32 GetLeftBoardTime();
	
	void SetCurrentRound(int32 value);
	int32 GetCurrentRound();
	
	int32 GetMaxRound();
	int32 GetMaxLeftBoardTime();
	
	UPROPERTY()
	FOnChangeBoardRound OnChangeBoardRoundDelegate;
};

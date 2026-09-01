#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BoardRPCPlayerPawn.generated.h"

class ABoardRPCPlayerActor;
class ABoardRPCActor;
class UBoardComponent;

// 이동 관련 변수 모음 구조체
USTRUCT()
struct FMoveData
{
	GENERATED_BODY()

public:
	// 도착 위치
	UPROPERTY()
	FVector GoalLocation;
	
	// 목표 지점 인덱스
	UPROPERTY()
	int GoalIndex = 0;
	
	// 방향 값 ( 1.f : 앞, -1.f : 뒤 )
	UPROPERTY()
	float DirValue = 1.f;
};

/*
* 플레이어가 빙의할 클래스로 고정된 시야를 제공한다.
 * 
 * ABoardPlayerPawn는 플레이어의 조종만 담당한다.
 * ABoardPlayerActor라고 하는 객체는 모드 상에서 사실상 말 (체스 킹) 같은 역할이며 월드 배치해서 보여주는 역할만한다. 
 * 실제 조작은 해당 클래스에서 담당한다.
 * 
 * 이동하라고 호출하면 이동 타이머가 생성되고 moveTime만큼 진행된다.
 */
UCLASS()
class RPC_TEST_API ABoardRPCPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	ABoardRPCPlayerPawn();

protected:
	virtual void BeginPlay() override;
	
	virtual void Destroyed() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	// BoradActor에서 생성된 플레이어 엑터 등록 
	void SetPlayerActor(ABoardRPCPlayerActor* actor);
	
	// 등록된 플레이어 엑터 가져오기
	ABoardRPCPlayerActor* GetPlayerActor();
	
	// 참조하는 액터 이동하는 클래스
	void MovePlayerActor(const FVector& location);	
	
	// 클라이언트가 서버에게 내 보드용 엑터 만들어 달라고 요청하는 함수 ( 새로 추가된 함수 )
	UFUNCTION(Server, Reliable)
	void ServerRequestSpawnBoard();
	
	UBoardComponent* GetBoardComponent();
	
	UPROPERTY(ReplicatedUsing=OnRep_MoveData)
	FMoveData MoveData;

	// MoveData Repnotify 함수
	UFUNCTION()
	void OnRep_MoveData();	
	
	// MoveData가 notify될 때 타이머를 등록하는 함수
	void StartMove();
		
	// bShouldMove = true로 함.
	void StartMoveFromData();
	
	// 월드에 있는 boardActor
	UPROPERTY(ReplicatedUsing=OnRep_BoardActor)
	TObjectPtr<ABoardRPCActor> BoardActor = nullptr;
	
	UFUNCTION()
	void OnRep_BoardActor();
	
protected:
	UFUNCTION(Server, Reliable)
	void ClearActor();
	
	// 해당 pawn이 boardActor에서 사용하는 액터
	UPROPERTY(ReplicatedUsing = OnRep_BoardPlayerActor)
	TObjectPtr<ABoardRPCPlayerActor> boardPlayerActor = nullptr;
	
	UFUNCTION()
	void OnRep_BoardPlayerActor();
	
	// 타이머에 등록할 이벤트 함수
	void UpdateMove();
	
	// 이동에 사용할 타이머 핸들
	FTimerHandle moveTimerHandle;	
	
	// note : 만약 노드 길이에 따라 일정하게 이동하고싶으면 moveTime * (사이 노드 개수) 하면 됨. 
	// timer에 사용하는 변수
	float startDistance = 0.f; // 시작 거리 값 저장용
	float goalDistance = 0.f; // 도착하는 거리 값 저장용
	float elapsedTime = 0.f; // 걸린시간
	float moveTickInterval = 1.f / 60.f; // 틱당 타이머 실행 길이
	float moveTime = 0.5f; // 움직이는 시간
	
	// 움직임 플래그 값, (Tick에서 확인)
	bool bShouldMove = false;
};

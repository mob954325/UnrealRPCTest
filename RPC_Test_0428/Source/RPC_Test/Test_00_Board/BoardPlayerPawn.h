#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BoardPlayerPawn.generated.h"

class ABoardActor;
class UBoardComponent;

/*
 * 플레이어가 빙의할 클래스로 고정된 시야를 제공한다.
 * 
 * ABoardPlayerPawn는 플레이어의 조종만 담당한다.
 * ABoardPlayerActor라고 하는 객체는 모드 상에서 사실상 말 (체스 킹) 같은 역할이며 월드 배치해서 보여주는 역할만한다. 
 * 실제 조작은 해당 클래스에서 담당한다.
 */
UCLASS()
class RPC_TEST_API ABoardPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	ABoardPlayerPawn();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// 생성된 BoardComponent 등록
	void SetBoardComponent(UBoardComponent* boardComp);
	UBoardComponent* GetBoardComponent();
	
	// BoradActor에서 생성된 플레이어 엑터 등록 
	void SetPlayerActor(AActor* actor);
	
	// 등록된 플레이어 엑터 가져오기
	AActor* GetPlayerActor();
	
	// 참조하는 액터 이동하는 클래스
	void MovePlayerActor(const FVector& location);
	
	TObjectPtr<ABoardActor> BoardActor = nullptr; // 스플라인 접근을 위해 추가함.
	int CurrentIndex = 0;
	
	TObjectPtr<AActor> boardPlayerActor = nullptr; // 임시
	
private:	
	TObjectPtr<UBoardComponent> boardComponent = nullptr;
	
	int userId = 0;		
};

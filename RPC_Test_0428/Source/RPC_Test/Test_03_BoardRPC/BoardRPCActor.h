#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "BoardRPCActor.generated.h"


class ABoardRPCPlayerActor;
class ABoardRPCNodeActor;
class UBoardComponent;
class USplineComponent;

/*
 * 월드에 보드 내용을 스폰할 클래스
 * 
 * 월드에 배치하고 게임 시작 시 등록된 데이터로 구성
 */
UCLASS()
class RPC_TEST_API ABoardRPCActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABoardRPCActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	// 여기 단계에서는 BP 초기화 훅이라고함.
	virtual void OnConstruction(const FTransform& Transform) override;	
	
	// 서버가 어떤 플레이어가 접속하는지 알고 있을 때, 그 플레이어의 전용 BoardPlayerActor를 월드에 생성
	// 그리고 해당 플레이어의 Pawn과 연결하는 서버 전용 처리 함수
	void SpawnUserActorForController(APlayerController* newPlayerController);
	
	// 인덱스로 splinePoint와 거리 값 반환 하는 함수
	bool GetSplineTransformByIndex(int index, FSplinePoint& outSplineTransform, float& outDistance);
	
	USplineComponent* GetSplineComponent();
	
	UBoardComponent* GetBoardComponent();	
	
	// 스플라인 컴포넌트에 노드 액터를 생성하는 함수
	void SpawnNodeActorsFromSpline();
	
	// 월드에 표시할 노드 엑터 클래스
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABoardRPCNodeActor> BoardNodeActorClass;
	
	// 픞레이어가 움직일 액터 ( 체스판의 말 같은 개념 )
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> BoardPlayerActorClass;	
private:
	// 참조하고 있는 보드 컴포넌트 -> 생성자에서 호출됨
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Board", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UBoardComponent> boardComponent;
	
	// 생성된 노드 액터들 
	TArray<TObjectPtr<ABoardRPCNodeActor>> nodeActors;
	
	// 생성될 플레이어 액터
	TArray<TObjectPtr<ABoardRPCPlayerActor>> playerActor;	
	
	// BoardActor.h -> 클래스 본인의 씬 루트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Board", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USceneComponent> SceneRoot;

	// 해당 클래스에서 스플라인 컴포넌트 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Board", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USplineComponent> splineComponent;

	// 데이터 변경 시 bp에서 보여줄 스플라인 컴포넌트 재구성
	// NOTE : 버튼 누르면 초기화 되는게 목적인데 반응을 안함.
	UFUNCTION(CallInEditor, Category="Board")
	void RebuildBoardPreview();

	UPROPERTY(EditAnywhere, Category="Board|Spline")
	bool bRebuildSplineFromDataInConstruction = true;
	
	// 데이터를 통해 스플라인 컴포넌트를 구성하는 함수
	void RebuildSplineFromData();
	
	// 컨트를러에 등록된 Pawn의 boardPlayerActor를 찾는 함수
	AActor* GetPlayerActorByController(APlayerController* newPlayerController);
};

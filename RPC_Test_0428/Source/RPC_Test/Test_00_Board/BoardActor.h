/*
 * UBoardComponent를 생성해서 해당 내용을 가지고 World에 Actor를 생성한다.
 * 
 * boardComponent의 CreateBoard를 BeginPlay에 호출하고 해당 내용을 기반으로 밑의 액터를 생성한다.
 * 
 * BoardNodeActorClass		: 월드에 생성할 각 노선도에 대한 역을 표시할 엑터
 * BoardPlayerActorClass	: 월드에 생성할 플레이어 엑터
 * 
 * 1. SplineComponent 자동 구성
 * 생성된 boardComponent에서 SubwayLineDatas내용을 넣는다.
 * 그리고 bRebuildSplineFromDataInConstruction을 딸깍(말 그대로 값변화면 주면됨)하면 OnConstruction에서 
 * boardComponent의 SubwayLineDatas을 가지고 SplinePoint를 구성한다. 생성된 노드를 가지고 위치를 원하는대로 조절하면 그대로 월드에 표시. 
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"

#include "BoardActor.generated.h"


class ABoardNodeActor;
class UBoardComponent;
class USplineComponent;

/*
 * BoardComponent가 붙어 있는 액터
 * 
 * 해당 클래스는 BoardComponent를 가지고 월드에 보드를 구성한다.
 * bRebuildSplineFromDataInConstruction의 값 변화를 통해 DataAsset를 확인하고 SplineComponent를 재구성한다. (OnConstruction 호출을 이용한 트릭 )
 */
UCLASS()
class RPC_TEST_API ABoardActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABoardActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	// 여기 단계에서는 BP 초기화 훅이라고함.
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// 월드에 표시할 노드 엑터 클래스
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> BoardNodeActorClass;
	
	// 픞레이어가 움직일 액터 ( 체스판의 말 같은 개념 )
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> BoardPlayerActorClass;	
	
	// 플레이어 생성
	UFUNCTION(BlueprintCallable)
	void SpawnUserActor();
	
	bool GetSplineTransform(int index, FSplinePoint& splineTransform, float& outDistance);
	USplineComponent* GetSplineComponent();
	
private:
	// 참조하고 있는 보드 컴포넌트 -> 생성자에서 호출됨
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Board", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UBoardComponent> boardComponent;
	
	// 생성된 노드 액터들 
	TArray<ABoardNodeActor*> nodeActors;
	
	// 생성될 플레이어 액터
	TArray<AActor*> playerActor;	
	
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
	
	// 스플라인 컴포넌트에 노드 액터를 생성하는 함수
	void SpawnNodeActorsFromSpline();
};

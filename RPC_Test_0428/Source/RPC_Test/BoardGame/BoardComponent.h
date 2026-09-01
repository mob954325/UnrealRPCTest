/*
 * UBoardComponent는 Actor에 부착할 수 있게 하기위해 ActorComponent를 베이스로 만든다.
 * UI로 붙이든 3D로 붙이든 어딘가에는 보드 내용을 추가해야하는데 일단 생각나는게 없어서 Actor에 부착한다고 생각하고 만든다.
 *	
 *	- 전체 구조 -
 *	ABoardActor나 위젯 : UBoardComponent를 가지고 실제 월드를 구성하는 클래스
 *	
 *	UBoardComponent					: 보드 데이터 총괄 관리
 *	
 *	SubwayLineDatas (AssetData)		: 각 지하철 노선을 배열로 가지고 있음.
 *		FSubwayNodeData (FStruct)	: 지하철 노선의 각 역에 대한 정적 데이터
 *	FSlotData (FStruct)				: 런타임 노드 데이터 (진입여부 등등) 
 *	FPlayerData (FStruct)			: 플레이어 데이터 (Id 등등)-> 아직 사용 안함.
 */

/*
 * todo
 * 프로젝트에서 추가한 목록
 * 보드 완성하기 (00 레벨)
 * [x] 랜덤 노드 생성
 * [x] Player 생성
 * [x] PlayerController 생성
 * [x] Spline 컴포넌트로 노드 연결
 * [x] 플레이어 스폰
 * [x] 플레이어 클릭 이동
 * [x] 스폰한 플레이어가 Spline Point에 따라 이동하는 기능 추가
 *  --> 위 내용 자체는 260417에 완료.
 * 통신 테스트 (02 레벨) 
 * [x] RPC 되는지 테스트
 * [x] RPC로 이동 확인
 * 보드 통신 추가하기
 * [] 한 보드안에 플레이어 추가 및 위치 동기화
 * [] 위 상황에서 플레이어 이동 ( 겹침 여부 x)
 * [] 보드 규칙 추가해서 적용 ( 턴, 이동 범위 등등)
 * 
 *  ---
 *  260415 : MEMO
 *  뭔가 BoardComponent에 Map으로 Actor와 Data가 묶여있는게 마음에 안듬.
 *  각 액터가 데이터를 들고 있어야할 거 같은데 뭔가 이게 UI로 표시하거나 월드에 액터로 표시할 때 필요한 데이터 덩어리로 만들고 싶은데 
 *  감이 안잡혀서 일단 Actor로 보이게 만듦.
 *  
 *  260416 : 구조 리펙터링
 *  UBoardComponent는 단순한 데이터 처리를 담당한다.
 *  구성하는 함수는 이제 ABoardActor나 위젯 클래스에 따로 추가한다.
 */


#pragma once

#include "CoreMinimal.h"
#include "BoardNodeData.h"
#include "BoardUserData.h"
#include "Components/ActorComponent.h"
#include "BoardComponent.generated.h"

class ABoardNodeActor;
class ABoardPlayerPawn;

/*
 * 보드 내용을 담고 있는 클래스
 * FBoardNodeData, FBoardUserData를 관리 한다.
 * 
 * 함수 내용을 부착할 블루 프린트에 연결해 사용한다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RPC_TEST_API UBoardComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBoardComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// 에셋 데이터를 기준으로 보드 데이터 생성
	// nodeDatas를 만든다.
	void CreateBoard();
	
	// 해당 보드를 사용할 유저 등록
	UFUNCTION(BlueprintCallable)
	void AddBoardUserData(FBoardUserData newData);
	
	// 등록된 플레이어 데이터를 가져오는 함수
	UFUNCTION(BlueprintCallable)	
	bool GetBoardUserData(int playerId, FBoardUserData& outData);
	
	// 데이터에셋을 기준으로한 노드 데이터
	void AddBoardNodeData(FBoardNodeData newData);
	
	// 노드 가져오는 함수
	UFUNCTION(BlueprintCallable)	
	bool GetBoardNodeData(int index, FBoardNodeData& outData);
	
	// 보드에서 사용하는 모든 노드 데이터를 TArray로 가져오는 함수
	UFUNCTION(BlueprintCallable)
	const TArray<FBoardNodeData>& GetAllBoardNodeData();
	
	// 보드에서 사용하는 모든 유저 데이터를 TArray로 가져오는 함수
	UFUNCTION(BlueprintCallable)
	const TArray<FBoardUserData>& GetAllUserData();
	
	// 지하철 노선도 에셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TArray<USubwayLineData*> SubwayLineDatas;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	
	// 슬롯 데이터 배열
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UPROPERTY(ReplicatedUsing=OnRep_NodeDatas)
	TArray<FBoardNodeData> nodeDatas;
	
	UFUNCTION()
	void OnRep_NodeDatas();
	
	// 플레이어 데이터
	UPROPERTY(ReplicatedUsing=OnRep_UserDatas)
	TArray<FBoardUserData> userDatas;
	
	UFUNCTION()
	void OnRep_UserDatas();
};

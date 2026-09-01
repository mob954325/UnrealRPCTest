#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardRPCNodeActor.generated.h"

class UTextRenderComponent;

/*
 * 기존 BoardNodeActor랑 동일한 클래스
 * 노드 메쉬와 인덱스 값을 표시하는 클래스
 */
UCLASS()
class RPC_TEST_API ABoardRPCNodeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABoardRPCNodeActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	// 모델 위에 띄울 텍스트 설정
	void SetWorldText(FText text);
	
	// 노드 인덱스 설정
	UFUNCTION(Client, Reliable)
	void SetNodeIndex(int newIndex);
	
	int GetNodeIndex();
	
protected:
	// 보드에서의 인덱스 값
	// 모든 플레이어가 알아야하므로 리플리케이트 추가
	UPROPERTY(ReplicatedUsing=OnRep_NodeIndex)
	int nodeIndex = -1;
	
	UFUNCTION()
	void OnRep_NodeIndex();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTextRenderComponent> WorldText;

};

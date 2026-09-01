#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardRPCPlayerActor.generated.h"

/*
 * 보드에서 플레이어가 사용하는 액터
 * 
 * 단순히 메쉬만 렌더링함.
 */
UCLASS()
class RPC_TEST_API ABoardRPCPlayerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABoardRPCPlayerActor();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	UPROPERTY(EditDefaultsOnly, Category="Material")
	TObjectPtr<UMaterialInterface> LocalPlayerMaterial = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category="Material")
	TObjectPtr<UMaterialInterface> OtherPlayerMaterial = nullptr;
	
	// 로컬 클라이언트와 다른 클라이언트 머터리얼을 구별해서 변경하는 함수
	void SetMaterial(bool isLocal);
};

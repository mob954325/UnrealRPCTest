#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardNodeActor.generated.h"

class UTextRenderComponent;

UCLASS()
class RPC_TEST_API ABoardNodeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABoardNodeActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	// boardComponent에서의 인덱스 
	UPROPERTY(BlueprintReadOnly)
	uint8 NodeIndex = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTextRenderComponent> WorldText;

	void SetWorldText(FText text);
};

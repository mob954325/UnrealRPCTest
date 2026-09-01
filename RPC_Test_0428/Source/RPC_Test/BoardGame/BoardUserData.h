#pragma once

#include "CoreMinimal.h"
#include "BoardUserData.generated.h"

/**
 * BoardComponent가 사용할 플레이어 데이터
 */
USTRUCT(BlueprintType)
struct FBoardUserData
{
	GENERATED_BODY()
	
	FBoardUserData() {}
	FBoardUserData(int id) : PlayerId(id) {}

	UPROPERTY(BlueprintReadOnly)
	int PlayerId;
};

#pragma once

#include "CoreMinimal.h"
#include "SubwayLineData.h"
#include "BoardNodeData.generated.h"

/**
 * BoardComponent의 각 슬롯 데이터
 * 
 * 런타임에서 필요한 데이터 모음
 */
USTRUCT(BlueprintType)
struct FBoardNodeData
{
	GENERATED_BODY()
	
	// 해당 슬롯에 플레이어가 있는지 여부
	UPROPERTY(Editanywhere, BlueprintReadWrite)
	bool IsPlayerIn = false;
	
	// 해당 슬롯으로 진입할 수 있는지 여부
	UPROPERTY(Editanywhere, BlueprintReadWrite)
	bool IsMoveable = true;	
	
	// 에셋 데이터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FSubwayNodeData baseData;
};

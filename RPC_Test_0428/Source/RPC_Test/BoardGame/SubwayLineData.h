// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SubwayLineData.generated.h"

/*
 * 지하철 각 역에 해당하는 데이터를 가지고 있는 구조체
 * 구역 이름, 연결 정보, 접근하기 위한 노드 인덱스
 */
USTRUCT(BlueprintType)
struct FSubwayNodeData
{
	GENERATED_BODY()

	// 역 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString AreaName;

	// 이전에 연결된 역 인덱스
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<int32> PrevConnectedIndices;

	// 이 다음 연결된 역 인덱스
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<int32> NextConnectedIndices;

	// 역 인덱스
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 NodeIndex = 0;
};

/**
 * 지하철 호선과 일치하는 내용이 들어가 있는 데이터 메셋 클래스
 */
UCLASS()
class RPC_TEST_API USubwayLineData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FSubwayNodeData> nodeDatas;
};

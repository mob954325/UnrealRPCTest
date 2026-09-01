#include "BoardActor.h"

#include "BoardNodeActor.h"
#include "BoardPlayerPawn.h"
#include "RPC_Test/BoardGame/BoardComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SplineComponent.h"

ABoardActor::ABoardActor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneRoot);

	boardComponent = CreateDefaultSubobject<UBoardComponent>(TEXT("BoardComponent"));

	// todo 갈랫길 일 때 다시 재구성
	splineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("GeneratedSpline"));
	splineComponent->SetupAttachment(SceneRoot);

	// 필요하면 디버그 표시
	splineComponent->SetDrawDebug(true);

	// 에디터에서 Construction Script와 같이 다루고 싶으면 프로젝트 상황에 맞게 사용
	// splineComponent->bInputSplinePointsToConstructionScript = true; -> 에디터에서 편집하고싶기 때문에 false
}

void ABoardActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if (bRebuildSplineFromDataInConstruction)
	{
		RebuildSplineFromData();		
	}
}

void ABoardActor::BeginPlay()
{
	Super::BeginPlay();
	
	boardComponent->CreateBoard();	// 런타임 데이터 보장
	// RebuildSplineFromData();		 // 재동기화
	SpawnNodeActorsFromSpline();    // 실제 게임 액터 스폰 - 노드
	SpawnUserActor();				// 실제 게임 엑터 스폰 - 플레이어 
}

void ABoardActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABoardActor::SpawnUserActor()
{
	// 노드가 생성된 이후에 해당 함수를 실행해야한다.
	// 해당 함수는 한번만 실행되어야한다.
	// 생성된 노드에 랜덤한 위치에 플레이어 액터가 스폰된다.
	
	if (nodeActors.IsEmpty()) return; // 배열없음
	
	// 랜덤 배치
	int batchIndex = UKismetMathLibrary::RandomIntegerInRange(0, nodeActors.Num() - 1);
	
	// 스폰 매개변수
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 물리에 상관없이 스폰
	
	// 일단 한 명만 생성
	FTransform spawnTransform = nodeActors[batchIndex]->GetTransform();
	AActor* spawnActor = GetWorld()->SpawnActor<AActor>(
		BoardPlayerActorClass,
		spawnTransform.GetLocation(),
		spawnTransform.GetRotation().Rotator(),
		spawnParams
	);
	
	if (spawnActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnUserActor : SpawnActor가 nullptr임"));	
	}
	
	playerActor.Add(spawnActor); // 배열에 추가
	
	// todo RPC 추가해서 플레이어가 여러 명이면 밑에 코드 수정해야함.
	// Pawn에 액터 참조 하는거
	// 이걸 빙의 될 때 해야하나? 
	
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (playerController == nullptr) return;
	
	APawn* pawn = playerController->GetPawn();
	if (pawn == nullptr) return;
	
	ABoardPlayerPawn* playerPawn = Cast<ABoardPlayerPawn>(pawn);
	if (playerPawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnUserActor : playerPawn 캐스팅 안됨."));		
	}
	
	playerPawn->SetPlayerActor(spawnActor);
	playerPawn->SetBoardComponent(boardComponent);
	playerPawn->BoardActor = this;
	playerPawn->CurrentIndex = batchIndex;
	
	UE_LOG(LogTemp, Error, TEXT("--- Player Actor Spawned ---"));
}

bool ABoardActor::GetSplineTransform(int index, FSplinePoint& splineTransform, float& outDistance)
{
	if (index < 0 || index >= nodeActors.Num()) return false;	
	splineTransform = splineComponent->GetSplinePointAt(index, ESplineCoordinateSpace::World);
	outDistance = splineComponent->GetDistanceAlongSplineAtSplinePoint(index);
	
	return true;
}

USplineComponent* ABoardActor::GetSplineComponent()
{
	return splineComponent;
}

void ABoardActor::RebuildBoardPreview()
{
	RebuildSplineFromData();
	UE_LOG(LogTemp, Warning, TEXT("RebuildBoardPreview called"));
}

void ABoardActor::RebuildSplineFromData()
{
	if (!splineComponent || !boardComponent) return;
	UE_LOG(LogTemp, Warning, TEXT("RebuildSplineFromData called"));
	
	// 데이터 가져오기
	TArray<USubwayLineData*> nodeDatas = boardComponent->SubwayLineDatas; // note : editor에도 접근 가능한 데이터이여야함.
	splineComponent->ClearSplinePoints(false);
	
	// 배열 순서대로 spline노드 추가
	for (int32 i = 0; i < nodeDatas.Num(); i++)
	{
		USubwayLineData* Data = nodeDatas[i];
		
		for (int32 j = 0; j < nodeDatas[i]->nodeDatas.Num(); j++)
		{
			FSplinePoint Point;	
			Point.InputKey = j;
			Point.Position = FVector( i * 50 + j * 50, 0.f, 0.f);
			Point.Rotation = FRotator::ZeroRotator;
			Point.Scale = FVector(1.f);
			Point.Type = ESplinePointType::Linear;

			splineComponent->AddPoint(Point, false);
		}
	}
	
	splineComponent->UpdateSpline(); // 스플라인 갱신?
}

void ABoardActor::SpawnNodeActorsFromSpline()
{
	// 스폰 매개변수
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 물리에 상관없이 스폰	
	
	for (int32 i = 0; i < splineComponent->GetNumberOfSplinePoints(); i++)
	{
		// 스폰 위치 찾은 후 스폰 위치에 액터 생성
		const FVector spawnVector = splineComponent->GetSplinePointAt(i, ESplineCoordinateSpace::World).Position;
		const FRotator spawnRotator = splineComponent->GetSplinePointAt(i, ESplineCoordinateSpace::World).Rotation;
		ABoardNodeActor* spawnActor = GetWorld()->SpawnActor<ABoardNodeActor>(
			BoardNodeActorClass,
			spawnVector,
			FRotator::ZeroRotator,
			spawnParams
			);		
		
		if (spawnActor == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("BoardActor : 액터 스폰 실패"));
			continue;			
		}
		
		spawnActor->NodeIndex = i; // todo 이거 AssetData nodeIndex값이랑 동일해야 정상작동함.
		nodeActors.Add(spawnActor);
		spawnActor->SetWorldText(FText::AsNumber(spawnActor->NodeIndex));
		
		UE_LOG(LogTemp, Log, TEXT("Spawn Success: %s"), *spawnActor->GetName());
	}
}


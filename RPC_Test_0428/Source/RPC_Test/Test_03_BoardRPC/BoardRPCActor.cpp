#include "BoardRPCActor.h"

#include "BoardRPCNodeActor.h"
#include "BoardRPCPlayerActor.h"
#include "BoardRPCPlayerPawn.h"
#include "BoardRPCPlayerState.h"
#include "RPC_Test/BoardGame/BoardComponent.h"
#include "Components/SplineComponent.h"

ABoardRPCActor::ABoardRPCActor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	bAlwaysRelevant = true;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneRoot);

	boardComponent = CreateDefaultSubobject<UBoardComponent>(TEXT("BoardComponent"));
	boardComponent->SetIsReplicated(true);
	
	// todo 갈랫길 일 때 다시 재구성
	splineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("GeneratedSpline"));
	splineComponent->SetupAttachment(SceneRoot);

	// 필요하면 디버그 표시
	splineComponent->SetDrawDebug(true);
}

void ABoardRPCActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if (bRebuildSplineFromDataInConstruction)
	{
		RebuildSplineFromData();		
	}
}

void ABoardRPCActor::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("BoardActor BeginPlay HasAuthority=%d ActorRep=%d CompRep=%d"),
		HasAuthority(),
		GetIsReplicated(),
		boardComponent ? boardComponent->GetIsReplicated() : 0);

	if (!HasAuthority())
	{
		return;
	}

	boardComponent->CreateBoard();

	UE_LOG(LogTemp, Warning, TEXT("[Server] CreateBoard done. nodeDatas.Num=%d"),
		boardComponent->GetAllBoardNodeData().Num());

	ForceNetUpdate();
	SpawnNodeActorsFromSpline();
}

void ABoardRPCActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABoardRPCActor::SpawnUserActorForController(APlayerController* newPlayerController)
{	
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("ABordActor : 서버에서만 실행 가능"));
		return;
	}
	
	if (newPlayerController == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ABordActor : 플레이어 컨트롤러가 유효하지 않습니다."));
		return;
	}
	
	if (BoardPlayerActorClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ABordActor : BoardPlayerActorClass가 등록되어있지 않습니다."));
		return;
	}
	
	if (nodeActors.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("ABordActor : 보드 엑터에 생성된 노드 액터가 없습니다."));
		return; // 배열없음
	}
	
	// 같은 컨트롤러에 대한 스폰 방지
	if (GetPlayerActorByController(newPlayerController) != nullptr)
	{			
		UE_LOG(LogTemp, Error, TEXT("ABordActor : 이미 생성된 플레이어 엑터가 있습니다."));
		return;			
	}
	
	// 컨트롤러 Pawn 찾기
	APawn* Pawn = newPlayerController->GetPawn();
	if (Pawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnUserActorForController : Controller Pawn is nullptr"));
		return;
	}

	ABoardRPCPlayerPawn* playerPawn = Cast<ABoardRPCPlayerPawn>(Pawn);
	if (playerPawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnUserActorForController : Pawn cast to ABoardPlayerPawn failed"));
		return;
	}
	
	// 플레이어 둘 노드를 랜덤으로 고르기
	const int32 spawnIndex = FMath::RandRange(0, nodeActors.Num() - 1);
	
	FSplinePoint spawnSplinePoint = FSplinePoint();
	float spawnDistance = 0.f;
	if (!GetSplineTransformByIndex(spawnIndex, spawnSplinePoint,spawnDistance))
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnUserActorForController :GetSplineTransformByIndex is failed"));		
	}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = newPlayerController; // owner 설정 - 액터를 owner을 contorller의 폰으로 지정
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	// boardPlayerActor 생성
	if (BoardPlayerActorClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnUserActorForController :BoardPlayerActorClass is empty"));
		return;
	}
	
	ABoardRPCPlayerActor* spawnedPlayerActor = GetWorld()->SpawnActor<ABoardRPCPlayerActor>(
		BoardPlayerActorClass,
		spawnSplinePoint.Position,
		spawnSplinePoint.Rotation,
		SpawnParams
		);
	
	if (spawnedPlayerActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnUserActorForController : SpawnActor returned nullptr"));
		return;
	}
	
	// 혹시 모를 boardActor 리플리케이션 설정 
	spawnedPlayerActor->SetReplicates(true);
	spawnedPlayerActor->SetMaterial(newPlayerController->IsLocalController());
	
	playerActor.Add(spawnedPlayerActor); // 배열에 추가
	
	// Pawn에 보드 정보 연결
	playerPawn->BoardActor = this;
	playerPawn->SetPlayerActor(spawnedPlayerActor);	
	playerPawn->MoveData.GoalLocation = spawnSplinePoint.Position;
    playerPawn->MoveData.GoalIndex = spawnIndex;
	
	ABoardRPCPlayerState* playerState = Cast<ABoardRPCPlayerState>(playerPawn->GetPlayerState());
	if (playerState != nullptr)
	{
		playerState->SetCurrentIndex(spawnIndex);
	}
}

bool ABoardRPCActor::GetSplineTransformByIndex(int index, FSplinePoint& outSplineTransform, float& outDistance)
{
	const int32 numPoints = splineComponent ? splineComponent->GetNumberOfSplinePoints() : 0;
	if (index < 0 || index >= numPoints) return false;	
	
	outSplineTransform = splineComponent->GetSplinePointAt(index, ESplineCoordinateSpace::World);
	outDistance = splineComponent->GetDistanceAlongSplineAtSplinePoint(index);
	
	return true;
}

USplineComponent* ABoardRPCActor::GetSplineComponent()
{
	return splineComponent;
}

UBoardComponent* ABoardRPCActor::GetBoardComponent()
{
	return boardComponent;
}

void ABoardRPCActor::RebuildBoardPreview()
{
	RebuildSplineFromData();
	UE_LOG(LogTemp, Warning, TEXT("RebuildBoardPreview called"));
}

void ABoardRPCActor::RebuildSplineFromData()
{
	if (!splineComponent || !boardComponent) return;
	UE_LOG(LogTemp, Warning, TEXT("RebuildSplineFromData called"));	
	
	// 데이터 가져오기
	TArray<USubwayLineData*> nodeDatas = boardComponent->SubwayLineDatas; // note : editor에도 접근 가능한 데이터이여야함.
	if (nodeDatas.IsEmpty()) return;
		
	splineComponent->ClearSplinePoints(false);
	
	// 배열 순서대로 spline노드 추가
	for (int32 i = 0; i < nodeDatas.Num(); i++)
	{
		USubwayLineData* subwayLineData = nodeDatas[i];
		if (subwayLineData == nullptr) continue;
		
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

AActor* ABoardRPCActor::GetPlayerActorByController(APlayerController* newPlayerController)
{
	if (playerActor.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("ABoardRPCActor::GetPlayerActorByController : playerActor 배열이 비어 있습니다."));		
		return nullptr;
	}
	
	for (ABoardRPCPlayerActor* ExistingActor : playerActor)
	{
		if (ExistingActor != nullptr && ExistingActor->GetOwner() == newPlayerController)
		{
			ABoardRPCPlayerPawn* ExistingBoardPawn = Cast<ABoardRPCPlayerPawn>(newPlayerController->GetPawn());
			if (ExistingBoardPawn != nullptr)
			{
				ExistingBoardPawn->SetPlayerActor(ExistingActor);
				return ExistingBoardPawn->BoardActor;
			}
		}
	}
	
	return nullptr;
}

void ABoardRPCActor::SpawnNodeActorsFromSpline()
{
	// 스폰 매개변수
	// 서버에서는 생성을 요청한다.
	// 클라이언트는 데이터 연결만 한다.
	
	if (!HasAuthority())
	{
		return;		
	}
	
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 물리에 상관없이 스폰	
	
	for (int32 i = 0; i < splineComponent->GetNumberOfSplinePoints(); i++)
	{
		// 스폰 위치 찾은 후 스폰 위치에 액터 생성
		const FVector spawnVector = splineComponent->GetSplinePointAt(i, ESplineCoordinateSpace::World).Position;
		const FRotator spawnRotator = splineComponent->GetSplinePointAt(i, ESplineCoordinateSpace::World).Rotation;
		ABoardRPCNodeActor* spawnActor = GetWorld()->SpawnActor<ABoardRPCNodeActor>(
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
	
		spawnActor->SetNodeIndex(i); // todo 이거 AssetSetWorldTextData nodeIndex값이랑 동일해야 정상작동함.
		nodeActors.Add(spawnActor);
	
		UE_LOG(LogTemp, Log, TEXT("Spawn Success: %s"), *spawnActor->GetName());
	}
}



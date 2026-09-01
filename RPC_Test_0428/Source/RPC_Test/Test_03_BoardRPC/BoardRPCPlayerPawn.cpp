#include "BoardRPCPlayerPawn.h"

#include "BoardRPCActor.h"
#include "BoardRPCPlayerActor.h"
#include "BoardRPCPlayerState.h"
#include "../BoardGame/BoardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ABoardRPCPlayerPawn::ABoardRPCPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void ABoardRPCPlayerPawn::BeginPlay()
{
	Super::BeginPlay();	
	
	// 로컬 폰이고, 한 번 생성되 었을 때 Board를 찾아서 호출한다.
	// 순서는 BoardActor가 생성되어 있어야한다.
	if (IsLocallyControlled())
	{
		ServerRequestSpawnBoard();		
	}
	
	if (boardPlayerActor != nullptr)
	{
		boardPlayerActor->SetMaterial(IsLocallyControlled());
	}
}

void ABoardRPCPlayerPawn::Destroyed()
{
	if (HasAuthority())
	{
		ClearActor();		
	}
	
	Super::Destroyed();	
}

void ABoardRPCPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABoardRPCPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABoardRPCPlayerPawn::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ABoardRPCPlayerPawn, BoardActor);
	DOREPLIFETIME(ABoardRPCPlayerPawn, boardPlayerActor);
	DOREPLIFETIME(ABoardRPCPlayerPawn, MoveData);
}

void ABoardRPCPlayerPawn::SetPlayerActor(ABoardRPCPlayerActor* actor)
{
	if (actor != nullptr)
	{
		boardPlayerActor = actor;
	}
	
	boardPlayerActor->SetMaterial(IsLocallyControlled());
}

ABoardRPCPlayerActor* ABoardRPCPlayerPawn::GetPlayerActor()
{
	return boardPlayerActor;
}

void ABoardRPCPlayerPawn::MovePlayerActor(const FVector& location)
{
	if (boardPlayerActor == nullptr) return;
	
	boardPlayerActor->SetActorLocation(location);
}

void ABoardRPCPlayerPawn::OnRep_BoardActor()
{
	UE_LOG(LogTemp, Warning, TEXT("BoardActor ready on client: %s"), *GetNameSafe(BoardActor));
}

UBoardComponent* ABoardRPCPlayerPawn::GetBoardComponent()
{
	return BoardActor ? BoardActor->GetBoardComponent() : nullptr;
}

void ABoardRPCPlayerPawn::OnRep_MoveData()
{
	StartMoveFromData();
}

void ABoardRPCPlayerPawn::StartMoveFromData()
{
	// 움직이기 전에 같은 인덱스를 클릭했으면 무시	
	StartMove();
	UE_LOG(LogTemp, Warning, TEXT("bShouldMove : True"));	
}

void ABoardRPCPlayerPawn::ServerRequestSpawnBoard_Implementation()
{
	// 보드 액터 찾기
	if (BoardActor == nullptr)
	{
		AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ABoardRPCActor::StaticClass());
		BoardActor = Cast<ABoardRPCActor>(FoundActor);
	}

	if (BoardActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerRequestSpawnBoardPlayerActor : BoardActor not found"));
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerRequestSpawnBoardPlayerActor : PlayerController is nullptr"));
		return;
	}

	BoardActor->SpawnUserActorForController(PlayerController); // 보드 플레이어 엑터 생성 요청
	
	// 머터리얼 색상 변경
	if (boardPlayerActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerRequestSpawnBoardPlayerActor : PlayerController is nullptr"));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("ServerRequestSpawnBoardPlayerActor : Spawn Player Actor"));
}

void ABoardRPCPlayerPawn::StartMove()
{
	ABoardRPCPlayerState* playerState = Cast<ABoardRPCPlayerState>(GetPlayerState());
	if (playerState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABoardRPCPlayerPawn : playerState is nullptr"));
		return;	
	}
	
	if (playerState->GetCurrentIndex() == MoveData.GoalIndex) return; // 이미 있는 위치면 무시
	
	USplineComponent* splineComp = BoardActor->GetSplineComponent();
	if (splineComp == nullptr) return;	
	
	if (boardPlayerActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABoardRPCPlayerPawn : BoardPlayerActor is nullptr"))
		return;
	}
	
	elapsedTime = 0.0f;	
	startDistance = splineComp->GetDistanceAlongSplineAtLocation(boardPlayerActor->GetActorLocation(), ESplineCoordinateSpace::World);
	goalDistance = splineComp->GetDistanceAlongSplineAtLocation(MoveData.GoalLocation, ESplineCoordinateSpace::World);
	bShouldMove = true;
	
	// 타이머 생성
	GetWorldTimerManager().ClearTimer(moveTimerHandle);
	GetWorldTimerManager().SetTimer(
		moveTimerHandle,
		this,
		&ABoardRPCPlayerPawn::UpdateMove,
		moveTickInterval, // 시간 
		true
		);
}

void ABoardRPCPlayerPawn::ClearActor_Implementation()
{
	if (boardPlayerActor == nullptr) return;
	boardPlayerActor->Destroy();
	// todo 플레이어 제거할 때 GameState, BoardRPCActor에서 참조하고 있는 내용 제거해야함.
}

void ABoardRPCPlayerPawn::OnRep_BoardPlayerActor()
{
	if (boardPlayerActor)
	{
		boardPlayerActor->SetMaterial(IsLocallyControlled());
	}
}

void ABoardRPCPlayerPawn::UpdateMove()
{
	if (!bShouldMove || BoardActor == nullptr || boardPlayerActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABoardRPCPlayerPawn::UpdateMove : bShouldMove == false, BoardActor == nullptr, boardPlayerActor == nullptr"));
		
		GetWorldTimerManager().ClearTimer(moveTimerHandle);		
		bShouldMove = false;
		return;
	}
	
	USplineComponent* splineComp = BoardActor->GetSplineComponent();
	if (splineComp == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABoardRPCPlayerPawn::UpdateMove : splineComp == nullptr"));
		
		GetWorldTimerManager().ClearTimer(moveTimerHandle);		
		return;
	}
	
	// 이동 시작	
	elapsedTime += moveTickInterval; // 진행 시간 갱신
	
	// 거리 구하기
	float alpha = FMath::Clamp(elapsedTime / moveTime, 0.f, 1.f);
	float currentDistance = FMathf::Lerp(startDistance, goalDistance, alpha);

	// 위치 갱신
	FTransform NewTransform = splineComp->GetTransformAtDistanceAlongSpline(currentDistance, ESplineCoordinateSpace::World);
	NewTransform.SetScale3D(FVector::OneVector);
	boardPlayerActor->SetActorTransform(NewTransform);
		
	// 근처면 정지
	if (alpha >= 1.f)
	{		
		if (ABoardRPCPlayerState* playerState = Cast<ABoardRPCPlayerState>(GetPlayerState()))
		{
			boardPlayerActor->SetActorLocation(MoveData.GoalLocation);	
			playerState->SetCurrentIndex(MoveData.GoalIndex);		
		}		
			
		bShouldMove = false;
		GetWorldTimerManager().ClearTimer(moveTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("bShouldMove : False"));
	}
}

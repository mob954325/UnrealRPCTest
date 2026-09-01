#include "BoardPlayerController.h"

#include "BoardActor.h"
#include "BoardNodeActor.h"
#include "BoardPlayerPawn.h"
#include "EnhancedInputComponent.h"
#include "Components/SplineComponent.h"
#include "RPC_Test/BoardGame/BoardComponent.h"
#include "RPC_Test/BoardGame/BoardNodeData.h"
#include "Kismet/KismetMathLibrary.h"

ABoardPlayerController::ABoardPlayerController()
{
	bShowMouseCursor = true; // 마우스 커서 보기 활성화
}

void ABoardPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	MoveBySpline();
}

void ABoardPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	bShowMouseCursor = true; // 마우스 커서 보기 활성화?
}

void ABoardPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
		
	if (!InputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("InputComponent is null in %s"), *GetName());
		return;
	}

	UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!enhancedInputComponent)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("InputComponent is not UEnhancedInputComponent. Check Project Settings > Engine > Input > Default Classes.")
		);
		return;
	}

	// 클릭 액션 추가
	if (ClickAction)
	{
		enhancedInputComponent->BindAction(
			ClickAction,
			ETriggerEvent::Started,
			this,
			&ABoardPlayerController::OnClick
		);
	}
}

void ABoardPlayerController::OnClick(const FInputActionValue& value)
{
	// 클릭한 위치로 이동
	UE_LOG(LogTemp, Warning, TEXT("Click"));
	
	// 판별한 오브젝트가 노드 엑터면 해당 Location으로 이동	
	
	FHitResult hitResult;		
	bool bIsHit = this->GetHitResultUnderCursorByChannel(
					UEngineTypes::ConvertToTraceType(ECC_Visibility), 
					false,
					hitResult);	
	
	if (!bIsHit) return;
	
	AActor* HitActor = hitResult.GetActor();
	if (!HitActor) return;
	
	// BoardNode Tag를 가진 오브젝트 인지 확인
	if (hitResult.GetActor()->ActorHasTag("BoardNode"))
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Result"));		
		
		// Board Node면 해당 위치로 이동
		ABoardNodeActor* nodeActor = Cast<ABoardNodeActor>(hitResult.GetActor());
		if (nodeActor == nullptr) return;
		
		// 나중에 노드 관련된 내용 가져오려면 밑에와 같이하면됨.
		// 보드 컴포넌트를 owner pawn에서 가져오기, 컨트롤러 owner는 beginplay 때 등록함.
		ABoardPlayerPawn* playerPawn = Cast<ABoardPlayerPawn>(GetPawn());
		if (playerPawn == nullptr) return;			
		
		UBoardComponent* boardComp = playerPawn->GetBoardComponent();
		
		// 해당 노드 찾아서 뭐 할 수 있음.
		FBoardNodeData outNodeData;
		boardComp->GetBoardNodeData(nodeActor->NodeIndex, outNodeData);
		UE_LOG(LogTemp, Warning, TEXT("Clicked node index is : %d"), outNodeData.baseData.NodeIndex); // 지금은 간단한 로그만 찍음
		
		// 임시 - 이동하기전에 최초 이동이면 값 갱신
		if (currnetDistance < 0)
		{
			FSplinePoint _;
			float firstDist;
			playerPawn->BoardActor->GetSplineTransform(playerPawn->CurrentIndex, _, firstDist);
			currnetDistance = firstDist;
		}
		
		// 말 이동
		FSplinePoint outPoint;
		if (playerPawn->BoardActor->GetSplineTransform(outNodeData.baseData.NodeIndex, outPoint, goalDistance))
		{			
			goalLocation = outPoint.Position;
			playerPawn->CurrentIndex = outNodeData.baseData.NodeIndex;		
			
			dirValue = goalDistance < currnetDistance ? -1.f : 1.f;
			
			bShouldMove = true;			
		}
	}	
}

void ABoardPlayerController::MoveBySpline()
{
	if (bShouldMove)
	{		
		// 보드 액터를 착지 위해서 폰 가져오기
		ABoardPlayerPawn* playerPawn = Cast<ABoardPlayerPawn>(GetPawn());
		if (playerPawn == nullptr) return;		
		
		// 연결된 SplineComponent 가져오기
		USplineComponent* splineComp = playerPawn->BoardActor->GetSplineComponent();
		if (splineComp == nullptr) return;
		
		// 현재 거리를 속력만큼 갱신
		currnetDistance += GetWorld()->GetDeltaSeconds() * 500.f * dirValue;
		
		// 현재 거리 값으로 현재 위치의 SplineTransform구하기
		FTransform newTransform = splineComp->GetTransformAtDistanceAlongSpline(currnetDistance, ESplineCoordinateSpace::World);
		newTransform.SetScale3D(FVector(1,1,1));		
		
		// 해당 위치로 이동
		playerPawn->boardPlayerActor->SetActorRelativeTransform(newTransform);
		
		// 만약 해당 위치에 도달했으면 플래그 비활성화
		// -> distance는 위치를 구하는 수단일 뿐, 플래그 해제 검사는 Location의 유사도를 검사.
		float remain = FVector::Distance(newTransform.GetLocation(), goalLocation);
		
		if (remain < 5.f)
		{
			playerPawn->boardPlayerActor->SetActorLocation(goalLocation);
			currnetDistance = goalDistance;
			bShouldMove = false;
			UE_LOG(LogTemp, Warning, TEXT("bShouldMove : False"));
		}
	}
}
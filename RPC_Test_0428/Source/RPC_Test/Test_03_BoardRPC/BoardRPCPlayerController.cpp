

#include "BoardRPCPlayerController.h"
#include "BoardRPCActor.h"
#include "BoardRPCGameState.h"
#include "BoardRPCNodeActor.h"
#include "BoardRPCPlayerPawn.h"
#include "BoardRPCPlayerState.h"
#include "BoardRPCWidgetBase.h"
#include "EnhancedInputComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "RPC_Test/BoardGame/BoardComponent.h"
#include "RPC_Test/BoardGame/BoardNodeData.h"

ABoardRPCPlayerController::ABoardRPCPlayerController()
{
	bShowMouseCursor = true; // 마우스 커서 보기 활성화
}


void ABoardRPCPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	bShowMouseCursor = true; // 마우스 커서 보기 활성화	
	
	// Player Board UI 추가
	if (BoardRPCWidget && IsLocalController())
	{
		BoardRPCWidgetInstance = CreateWidget<UBoardRPCWidgetBase>(
			this, 
			BoardRPCWidget, 
			TEXT("PlayerUI")
			);
		
		if (BoardRPCWidgetInstance)
		{
			BoardRPCWidgetInstance->AddToViewport();			
		}
	}
}

void ABoardRPCPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	// 임시 : 매 틱마다 갱신	
	ABoardRPCGameState* GS = Cast<ABoardRPCGameState>(GetWorld()->GetGameState());
	if (BoardRPCWidgetInstance && GS)
	{
		BoardRPCWidgetInstance->UpdateLeftBoardTime(GS->GetLeftBoardTime());
	}
}


void ABoardRPCPlayerController::SetupInputComponent()
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
			&ABoardRPCPlayerController::OnClick
		);
	}
}

void ABoardRPCPlayerController::OnClick(const FInputActionValue& value)
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
		ABoardRPCNodeActor* nodeActor = Cast<ABoardRPCNodeActor>(hitResult.GetActor());
		if (nodeActor == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("ABoardRPCPlayerController : HitResult is not ABoardRPCNodeActor class"));	
			return;
		}
		
		// 해당 노드 찾아서 뭐 할 수 있음.
		RequestMove(nodeActor->GetNodeIndex());
	}
}

bool ABoardRPCPlayerController::IsValidMove(const int targetIndex)
{
	// 이동 하려는 위치가 Playstate의 remainMoveCount값보다 크면 false
	
	ABoardRPCPlayerState* PS = GetPlayerState<ABoardRPCPlayerState>();
	if (PS == nullptr) return false;
	
	return abs(PS->GetCurrentIndex() - targetIndex) <= PS->GetRemainMoveCount();
}

void ABoardRPCPlayerController::RequestMove_Implementation(const int index)
{
	// 나중에 노드 관련된 내용 가져오려면 밑에와 같이하면됨.
	// 보드 컴포넌트를 owner pawn에서 가져오기, 컨트롤러 owner는 beginplay 때 등록함.
	ABoardRPCPlayerPawn* playerPawn = Cast<ABoardRPCPlayerPawn>(GetPawn());
	UE_LOG(LogTemp, Error, TEXT("ABoardRPCPlayerController : PawnName : %s"), *GetPawn()->GetName());			
	
	if (playerPawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABoardRPCPlayerController : PlayerPawn is Null"));			
		return;			
	}
	
	UBoardComponent* boardComp = playerPawn->GetBoardComponent();
	
	if (boardComp == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABoardRPCPlayerController : BoardComponent is Null"));
		return;
	}
	
	FBoardNodeData outNodeData;
	if (!boardComp->GetBoardNodeData(index, outNodeData))
	{
		UE_LOG(LogTemp, Error, TEXT("ABoardRPCPlayerController : outNodeData is empty"))
	}	
	
	ABoardRPCPlayerState* PS = GetPlayerState<ABoardRPCPlayerState>();
	if (PS == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ABoardRPCPlayerController : playerState is nullptr"))
		return;
	}	
	
	UE_LOG(LogTemp, Warning, TEXT("Clicked node index is : %d"), outNodeData.baseData.NodeIndex);
	
	if (outNodeData.baseData.NodeIndex == playerPawn->MoveData.GoalIndex)
	{
		UE_LOG(LogTemp, Error, TEXT("ABoardRPCPlayerController : 이미 해당 노드가 도착점임 index : %d"), outNodeData.baseData.NodeIndex);
		return;
	}
	
	if (!IsValidMove(index))
	{
		UE_LOG(LogTemp, Error, TEXT("ABoardRPCPlayerController : 움직일 수 없는 범위 입니다. Goal index %d"), outNodeData.baseData.NodeIndex);		
		return;
	}
	
	// 움직일 수 있으면 이동 횟수 차감
	PS->DecreaseMoveCount(abs(index - PS->GetCurrentIndex()));	
	
	// 말 이동
	FSplinePoint outPoint;
	float distance;
	if (playerPawn->BoardActor->GetSplineTransformByIndex(outNodeData.baseData.NodeIndex, outPoint, distance))
	{			
		playerPawn->MoveData.GoalLocation = outPoint.Position;
		playerPawn->MoveData.GoalIndex = index;
		playerPawn->MoveData.DirValue = index < PS->GetCurrentIndex() ? -1.f : 1.f;
	}
	
	// playerPawn->StartMoveFromData();
	playerPawn->StartMoveFromData();
}	

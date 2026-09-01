#include "BoardRPCPlayerState.h"

#include "BoardRPCGameState.h"
#include "Net/UnrealNetwork.h"

class ABoardRPCGameState;

void ABoardRPCPlayerState::BeginPlay()
{
	Super::BeginPlay();	
		
	ABoardRPCGameState* GS = Cast<ABoardRPCGameState>(GetWorld()->GetGameState());
	if (GS != nullptr)
	{
		GS->OnChangeBoardRoundDelegate.AddDynamic(this, &ABoardRPCPlayerState::ResetMovecountDelegateWrapper);
	}
	
	ResetMoveCount();
}

void ABoardRPCPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABoardRPCPlayerState, currentIndex);
}

int ABoardRPCPlayerState::GetCurrentIndex() const
{
	return currentIndex;
}

void ABoardRPCPlayerState::SetCurrentIndex(int index)
{
	UE_LOG(LogTemp, Warning, TEXT("Setting Current Index to %d"), index);
	currentIndex = index;
}

void ABoardRPCPlayerState::DecreaseMoveCount(int value)
{
	if (value < 0)
	{
		UE_LOG(LogTemp, Warning , TEXT("DecreaseMoveCount value is negative"));
		return;
	}
	
	if (remainMoveCount <= 0)
	{
		UE_LOG(LogTemp, Warning , TEXT("RemainMoveCount is 0"));		
		return;
	}
	
	remainMoveCount -= value;
}

int ABoardRPCPlayerState::GetRemainMoveCount() const
{
	return remainMoveCount;
}

void ABoardRPCPlayerState::ResetMoveCount()
{
	remainMoveCount = maxMoveCount;
}

void ABoardRPCPlayerState::ResetMovecountDelegateWrapper(int32 currentRound)
{
	ResetMoveCount();
}

void ABoardRPCPlayerState::OnRep_CurrentIndex()
{
	// UI 갱신 등등
}

void ABoardRPCPlayerState::OnRep_RemainMoveCount()
{
}

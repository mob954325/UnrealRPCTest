#include "BoardRPCGameState.h"
#include "Net/UnrealNetwork.h"

void ABoardRPCGameState::BeginPlay()
{
	Super::BeginPlay();
	
	LeftBoardTime = MaxBoardTime;
	CurrentRound = 1;
}

void ABoardRPCGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ABoardRPCGameState, LeftBoardTime);
	DOREPLIFETIME(ABoardRPCGameState, CurrentRound);
}

void ABoardRPCGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ABoardRPCGameState::StartTrun()
{
	
}

void ABoardRPCGameState::EndTurn()
{
}

void ABoardRPCGameState::OnRep_LeftBoardTime()
{
}

void ABoardRPCGameState::OnRep_CurrentRound()
{
	OnChangeBoardRoundDelegate.Broadcast(CurrentRound);
}

void ABoardRPCGameState::SetLeftBoardTime(int32 value)
{
	LeftBoardTime = value;
}

int32 ABoardRPCGameState::GetLeftBoardTime()
{
	return LeftBoardTime;
}

void ABoardRPCGameState::SetCurrentRound(int32 value)
{
	if (HasAuthority())
	{
		OnChangeBoardRoundDelegate.Broadcast(value);
	}
	
	CurrentRound = value;
}

int32 ABoardRPCGameState::GetCurrentRound()
{
	return CurrentRound;
}

int32 ABoardRPCGameState::GetMaxRound()
{
	return MaxRound;
}

int32 ABoardRPCGameState::GetMaxLeftBoardTime()
{
	return MaxBoardTime;
}

#include "BoardPlayerPawn.h"

#include "../BoardGame/BoardComponent.h"

ABoardPlayerPawn::ABoardPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABoardPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
	// 임시로 유저 아이디 지정
	userId = 0;
	// USERID++;
}

void ABoardPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABoardPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABoardPlayerPawn::SetBoardComponent(UBoardComponent* boardComp)
{
	if (boardComp != nullptr)
	{
		this->boardComponent = boardComp;		
	}
}

UBoardComponent* ABoardPlayerPawn::GetBoardComponent()
{
	return boardComponent;
}

void ABoardPlayerPawn::SetPlayerActor(AActor* actor)
{
	if (actor != nullptr)
	{
		boardPlayerActor = actor;
	}
}

AActor* ABoardPlayerPawn::GetPlayerActor()
{
	return boardPlayerActor;
}

void ABoardPlayerPawn::MovePlayerActor(const FVector& location)
{
	if (boardPlayerActor == nullptr) return;
	
	boardPlayerActor->SetActorLocation(location);
}

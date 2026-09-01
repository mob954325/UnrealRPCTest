#include "BoardRPCPlayerActor.h"

#include "BoardRPCPlayerPawn.h"
#include "RPC_Test/Test_00_Board/BoardPlayerPawn.h"

ABoardRPCPlayerActor::ABoardRPCPlayerActor()
{
	PrimaryActorTick.bCanEverTick = false;	
		
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");	
	RootComponent = Mesh;
}

void ABoardRPCPlayerActor::BeginPlay()
{
	Super::BeginPlay();	
}

void ABoardRPCPlayerActor::SetMaterial(bool isLocal)
{
	if (Mesh == nullptr) return;
	
	if (isLocal)
	{
		if (LocalPlayerMaterial == nullptr)
		{
			return;			
		}
		
		Mesh->SetMaterial(0, LocalPlayerMaterial);
	}
	else
	{
		if (OtherPlayerMaterial == nullptr)
		{
			return;			
		}
		
		Mesh->SetMaterial(0, OtherPlayerMaterial);	
	}
}

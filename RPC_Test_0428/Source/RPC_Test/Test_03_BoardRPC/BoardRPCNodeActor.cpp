#include "BoardRPCNodeActor.h"

#include "Components/TextRenderComponent.h"
#include "Net/UnrealNetwork.h"

ABoardRPCNodeActor::ABoardRPCNodeActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;
	
	WorldText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("WorldText"));
	WorldText->SetupAttachment(Mesh);
}

void ABoardRPCNodeActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (WorldText)
	{
		WorldText->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform);
		UE_LOG(LogTemp, Warning, TEXT("ABoardRPCNodeActor : Set WorldText"));
	}
}

void ABoardRPCNodeActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ABoardRPCNodeActor, nodeIndex);
}

void ABoardRPCNodeActor::OnRep_NodeIndex()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_NodeIndex --> Node Index : %d"), nodeIndex);
	SetWorldText(FText::AsNumber(nodeIndex)); // 리플리케이트된 값을 월드 텍스트에 반영
}

void ABoardRPCNodeActor::SetWorldText(FText text)
{
	if (WorldText)
	{
		WorldText->SetText(text);	
	}
}

void ABoardRPCNodeActor::SetNodeIndex_Implementation(int newIndex)
{
	nodeIndex = newIndex;
	
	if (HasAuthority())
	{
		SetWorldText(FText::AsNumber(nodeIndex));
	}
}

int ABoardRPCNodeActor::GetNodeIndex()
{
	return nodeIndex;
}

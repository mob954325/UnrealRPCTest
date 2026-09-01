#include "BoardNodeActor.h"

#include "Components/TextRenderComponent.h"

ABoardNodeActor::ABoardNodeActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;
	
	WorldText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("WorldText"));
	WorldText->SetupAttachment(Mesh);
}

void ABoardNodeActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (WorldText)
	{
		WorldText->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void ABoardNodeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABoardNodeActor::SetWorldText(FText text)
{
	if (WorldText)
	{
		WorldText->SetText(text);
	}
}


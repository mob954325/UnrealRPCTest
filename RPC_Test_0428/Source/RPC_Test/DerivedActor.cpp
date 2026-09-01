// Fill out your copyright notice in the Description page of Project Settings.


#include "DerivedActor.h"

ADerivedActor::ADerivedActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true; // 생성자에서 Replicates가 활성화 되어야한다고 적혀있음.	
	
	// 간단한 메쉬 생성
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMesh); // staticMesh를 루트로 지정
}

void ADerivedActor::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void ADerivedActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADerivedActor::ClientRPC_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("ClientRPC executed."))
}

void ADerivedActor::MulticastRPC_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("ServerRPC executed."))
}

void ADerivedActor::ServerRPC_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("MulticastRPC executed."))
}

/// 각 호출 함수
/// 	// Call from client to run on server
/// ADerivedClientActor* MyDerivedClientActor;
/// MyDerivedClientActor->ServerRPC();
///  
/// // Call from server to run on client
/// ADerivedServerActor* MyDerivedServerActor;
/// MyDerivedServerActor->ClientRPC();
///  
/// // Call from server to run on server and all relevant clients
/// ADerviedServerActor* MyDerivedServerActor;
/// MyDerievedServerActor->MulticastRPC();


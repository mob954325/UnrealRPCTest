// Fill out your copyright notice in the Description page of Project Settings.

// 참고한 링크
// https://dev.epicgames.com/documentation/unreal-engine/remote-procedure-calls-in-unreal-engine?application_version=5.3&lang=en-US

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DerivedActor.generated.h"

/*
 * RPC 테스트 액터 클래스
 */
UCLASS()
class RPC_TEST_API ADerivedActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADerivedActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	/// 각 RPC 함수는 Reliable 이나 Unreliable 을 무조건 작성하라고 Intellisense에 나타남
	/// 위 문서에는 안젹혀있으니 참고
	
	UFUNCTION(Client, Reliable)
	void ClientRPC();
	
	UFUNCTION(Server, Reliable)
	void ServerRPC();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> StaticMesh;
};

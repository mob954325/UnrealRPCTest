/*  
 *  todo
 *  [x] 플레이어 클라이언트 생성
 *  [x] 보드 리플리케이트 확인
 *  [x] 플레이어 리플리케이트 확인
 *  [x] 플레이어 이동 동기화 확인
 *  
 *  0423 
 *  현재 사용 안함.
 *  
 *  0428
 *  게임 모드는 보드의 규칙을 설정한다.
 *  타이머 진행과 라운드 진행 및 초기화
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BoardRPCGameMode.generated.h"

class ABoardRPCActor;
/**
 * 03_Test_BoardRPC에서 사용하는 GameMode
 * 
 * 보드 타이머와, 라운드 수를 갱신한다.
 */
UCLASS()
class RPC_TEST_API ABoardRPCGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ABoardRPCGameMode();
	
	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	
	virtual void BeginPlay() override;
	
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	virtual void StartPlay() override;
	
	// 라운드 타이머 설정 함수
	void SetBoardTimer();
	
	// 보드 타이머 정지 함수
	void StopBoardTimer();
	
	// 라운드 수 초기화 함수
	void ResetRound();
	
	// 시간 초기화
	void ResetTime();
	
	FTimerHandle BoardTimerHandle;
};

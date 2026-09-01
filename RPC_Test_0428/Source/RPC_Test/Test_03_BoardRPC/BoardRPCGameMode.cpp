#include "BoardRPCGameMode.h"

#include "BoardRPCGameState.h"
#include "BoardRPCPlayerPawn.h"
#include "kismet/KismetSystemLibrary.h"

ABoardRPCGameMode::ABoardRPCGameMode()
{
}

void ABoardRPCGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("InitGame: Pre"));
	UE_LOG(LogTemp, Warning, TEXT("InitGame: Pre"));
	Super::InitGame(MapName, Options, ErrorMessage);
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("InitGame: Post"));
	UE_LOG(LogTemp, Warning, TEXT("InitGame: Post"));
}

void ABoardRPCGameMode::BeginPlay()
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("BeginPlay: Pre"));
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay: Pre"));
	Super::BeginPlay();
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("BeginPlay: Post"));
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay: Post"));
	
	// 임시 : 20초 타이머 추가하기
	SetBoardTimer();
	ResetRound();
}

void ABoardRPCGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
                                 FString& ErrorMessage)
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("PreLogin: Pre"));
	UE_LOG(LogTemp, Warning, TEXT("PreLogin: Pre"));
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("PreLogin: Post"));
	UE_LOG(LogTemp, Warning, TEXT("PreLogin: Post"));
}

APlayerController* ABoardRPCGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal,
	const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Login: Pre"));
	UE_LOG(LogTemp, Warning, TEXT("Login: Pre"));
	return Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
}

void ABoardRPCGameMode::PostLogin(APlayerController* NewPlayer)
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("PostLogin : Pre"));
	UE_LOG(LogTemp, Warning, TEXT("PostLogin : Pre"));
	Super::PostLogin(NewPlayer);
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("PostLogin : Post"));
	UE_LOG(LogTemp, Warning, TEXT("PostLogin : Post"));
	
	// pawn에 actor 생성 요청
	// note : 이 때 pawn이 생성되 었을까? -> ㅇㅇ
	APawn* pawn = NewPlayer->GetPawn();
	if (pawn == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("PostLogin: Pawn is invalid"));
		return;
	}
	
	ABoardRPCPlayerPawn* playerPawn = Cast<ABoardRPCPlayerPawn>(pawn);
	if (playerPawn == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("PostLogin: Can not cat to ABoardRPCPlayerPawn"));
		return;
	}	
}

void ABoardRPCGameMode::StartPlay()
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("StartPlay : Pre"));
	UE_LOG(LogTemp, Warning, TEXT("StartPlay : Pre"));
	Super::StartPlay();
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("StartPlay : Post"));
	UE_LOG(LogTemp, Warning, TEXT("StartPlay : Post"));
}

void ABoardRPCGameMode::SetBoardTimer()
{
	ABoardRPCGameState* GS = Cast<ABoardRPCGameState>(GetWorld()->GetGameState());
	GetWorldTimerManager().SetTimer(
		BoardTimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			ABoardRPCGameState* GS = GetGameState<ABoardRPCGameState>();
			if (GS != nullptr)
			{
				int32 leftTime = GS->GetLeftBoardTime();
				leftTime--;
				GS->SetLeftBoardTime(leftTime); // 타이머 감소
				
				// 시간 다 되면 라운드 상승 or 게임 종료
				if (leftTime == 0)
				{					
					if (GS->GetCurrentRound() >= GS->GetMaxRound())
					{
						// 게임 종료
						UE_LOG(LogTemp, Warning, TEXT("Game Over !"));						
					}
					else
					{
						// 값 초기화 후 라운드 상승
						UE_LOG(LogTemp, Warning, TEXT("Next Round !"));					
						
						GS->SetCurrentRound(GS->GetCurrentRound() + 1);
						GS->SetLeftBoardTime(GS->GetMaxLeftBoardTime());
						// todo BoardCast : 모든 플레이어 보드 내용 초기화 
					}
				}
			}			
		}),
		1.0f, // 1초마다
		true, // 루프
		0.0f // 첫 딜레이 x
		);
}

void ABoardRPCGameMode::StopBoardTimer()
{
	GetWorldTimerManager().ClearTimer(BoardTimerHandle);
}

void ABoardRPCGameMode::ResetRound()
{
	ABoardRPCGameState* GS = Cast<ABoardRPCGameState>(GetWorld()->GetGameState());
	if (GS == nullptr) return;
	
	GS->SetCurrentRound(1);
}
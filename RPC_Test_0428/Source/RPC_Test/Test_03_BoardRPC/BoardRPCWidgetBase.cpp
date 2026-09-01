// Fill out your copyright notice in the Description page of Project Settings.


#include "BoardRPCWidgetBase.h"

#include "BoardRPCGameState.h"
#include "Components/TextBlock.h"

void UBoardRPCWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	ABoardRPCGameState* GS = Cast<ABoardRPCGameState>(GetWorld()->GetGameState());
	if (GS != nullptr)
	{
		GS->OnChangeBoardRoundDelegate.AddDynamic(this, &UBoardRPCWidgetBase::UpdateCurrnetRound);
		GS->SetCurrentRound(1);		
	}
}

void UBoardRPCWidgetBase::UpdateLeftBoardTime(int32 currTime)
{
	if (LeftBoardTime != nullptr)
	{
		LeftBoardTime->SetText(FText::AsNumber(currTime));
	}
}

void UBoardRPCWidgetBase::UpdateCurrnetRound(int32 currRound)
{
	if (CurrentRound != nullptr)
	{
		FString temp = FString::Printf(TEXT("Current Round : %d"), currRound);
		CurrentRound->SetText(FText::FromString(temp));
		
	}
}

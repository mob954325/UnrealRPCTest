

#include "BoardComponent.h"
#include "Net/UnrealNetwork.h"

UBoardComponent::UBoardComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // 틱 사용할거같지는 않아서 false 처리
	SetIsReplicatedByDefault(true);
}


void UBoardComponent::BeginPlay()
{
	Super::BeginPlay();	
}


void UBoardComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBoardComponent::CreateBoard()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}
	
	if (SubwayLineDatas.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("SubwayLineData가 비어 있습니다."));
		return;
	}	
	
	// 런타임에 사용할 데이터 생성
	UE_LOG(LogTemp, Warning, TEXT("SubwayLineDatas.Num() :  %d"),SubwayLineDatas.Num());
	for (int i = 0; i < SubwayLineDatas.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("SubwayLineDatas[i]->nodeDatas.Num() :  %d"),SubwayLineDatas[i]->nodeDatas.Num());
		for (int j = 0; j < SubwayLineDatas[i]->nodeDatas.Num(); j++)
		{			
			FBoardNodeData newData;
			newData.baseData = SubwayLineDatas[i]->nodeDatas[j];
			UE_LOG(LogTemp, Warning, TEXT("nodedat index %d"),newData.baseData.NodeIndex);
			AddBoardNodeData(newData);
		}		
	}
	
	UE_LOG(LogTemp, Warning, TEXT("[Server] CreateBoard done. nodeDatas.Num=%d"), nodeDatas.Num());

	if (AActor* Owner = GetOwner())
	{
		Owner->ForceNetUpdate();
	}
}

void UBoardComponent::AddBoardUserData(FBoardUserData newData)
{
	FBoardUserData data;
	if (GetBoardUserData(newData.PlayerId, data))
	{
		UE_LOG(LogTemp, Warning, TEXT("중복된 유저 ID 등록 시도, ID : %d"), newData.PlayerId);
		return;
	}
	
	userDatas.Add(newData);
}

bool UBoardComponent::GetBoardUserData(int playerId, FBoardUserData& outData)
{
	for (FBoardUserData& data : userDatas)
	{
		if (data.PlayerId == playerId)
		{
			outData = data;
			return true;
		}
	}	
	
	return false;
}

void UBoardComponent::AddBoardNodeData(FBoardNodeData newData)
{
	FBoardNodeData data;
	
	if (nodeDatas.IsEmpty())
	{
		nodeDatas.Add(newData);		
	}
	else
	{
		if (GetBoardNodeData(newData.baseData.NodeIndex, data))
		{
			UE_LOG(LogTemp, Warning, TEXT("중복된 노드 ID 등록 시도, ID : %d"),newData.baseData.NodeIndex);
			return;
		}
		
		nodeDatas.Add(newData);		
	}
}

bool UBoardComponent::GetBoardNodeData(int index, FBoardNodeData& outData)
{
	if (nodeDatas.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("GetBoardNodeData nodeDatas is empty"));		
		return false;
	}
	
	for (FBoardNodeData& data : nodeDatas)
	{		
		if (data.baseData.NodeIndex == index)
		{
			UE_LOG(LogTemp, Warning, TEXT("GetBoardNodeData : index %d"), data.baseData.NodeIndex);		
			outData = data;
			return true;
		}
	}
	
	return false;
}

const TArray<FBoardNodeData>& UBoardComponent::GetAllBoardNodeData()
{
	return nodeDatas;
}

const TArray<FBoardUserData>& UBoardComponent::GetAllUserData()
{
	return userDatas;
}

void UBoardComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBoardComponent, nodeDatas, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBoardComponent, userDatas, COND_None, REPNOTIFY_Always);
	
	//DOREPLIFETIME(UBoardComponent, nodeDatas);
	//DOREPLIFETIME(UBoardComponent, userDatas);
}

void UBoardComponent::OnRep_NodeDatas()
{
	UE_LOG(LogTemp, Warning, TEXT("[Client] OnRep_NodeDatas. nodeDatas.Num=%d"), nodeDatas.Num());		
}

void UBoardComponent::OnRep_UserDatas()
{
	UE_LOG(LogTemp, Warning, TEXT("UBoardComponent OnRep_UserDatas"));	
}

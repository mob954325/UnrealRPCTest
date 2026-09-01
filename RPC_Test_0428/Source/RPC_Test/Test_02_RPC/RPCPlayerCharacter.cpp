#include "RPCPlayerCharacter.h"
#include "RPCProjectile.h"
#include "Components/InputComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

ARPCPlayerCharacter::ARPCPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true; // 액터 자동 리플리케이트
	
	// 체력 초기화
	maxHealth = 100.0f;
	currentHealth = maxHealth;	
	
	//발사체 클래스 초기화
	// projectileClass = ARPCProjectile::StaticClass();
	
	//발사 속도 초기화
	FireRate = 0.25f;
	bIsFiringWeapon = false;
}

void ARPCPlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ARPCPlayerCharacter, currentHealth);
}

void ARPCPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ARPCPlayerCharacter::StartFire()
{
	if (!bIsFiringWeapon)
	{
		bIsFiringWeapon = true;
		UWorld* World = GetWorld();
		World->GetTimerManager().SetTimer(FiringTimer, this, &ARPCPlayerCharacter::StopFire, FireRate, false); // bIsFiringWeapon = false 타이머 
		HandleFire();
	}
}

void ARPCPlayerCharacter::StopFire()
{
	bIsFiringWeapon = false;
}

void ARPCPlayerCharacter::HandleFire_Implementation()
{
	FVector spawnLocation = GetActorLocation() + ( GetActorRotation().Vector()  * 100.0f ) + (GetActorUpVector() * 50.0f);
	FRotator spawnRotation = GetActorRotation();
 
	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;
 
	// 발사체 스폰
	ARPCProjectile* spawnedProjectile = GetWorld()->SpawnActor<ARPCProjectile>(projectileClass, spawnLocation, spawnRotation, spawnParameters);
}

void ARPCPlayerCharacter::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

void ARPCPlayerCharacter::OnHealthUpdate()
{
	// 클라이언트 전용 함수 기능
	if (IsLocallyControlled())
	{
		// 남은 체력 출력
		FString healthMessage = FString::Printf(TEXT("You now have %f health remaining."), currentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
 
		// 사망 출력
		if (currentHealth <= 0)
		{
			FString deathMessage = FString::Printf(TEXT("You have been killed."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);
		}
	}
	
	// 서버 전용 함수 기능
	if (GetLocalRole() == ROLE_Authority)
	{
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), currentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);         
	}
	
	/*
	 * 모든 머신에서 실행되는 함수
	 * 
	 * 대미지 또는 사망결과로 발생하는 특별 함수 기능 배치
	 */
}

void ARPCPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARPCPlayerCharacter::SetCurrentHealth(float healthValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		currentHealth = FMath::Clamp(healthValue, 0.f, maxHealth);
		OnHealthUpdate();
	}
}

float ARPCPlayerCharacter::TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{	
	float damageApplied = currentHealth - DamageTaken;
	SetCurrentHealth(damageApplied);
	
	return damageApplied;
}

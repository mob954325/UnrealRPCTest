#include "RPCProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ARPCProjectile::ARPCProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	//발사체와 콜리전의 루트 컴포넌트 역할을 할 SphereComponent 정의
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	SphereComponent->InitSphereRadius(37.5f);							// 스피어 크기 초기화
	SphereComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));	// 콜리전 프로필 타입 설정
	RootComponent = SphereComponent;
	
	//비주얼 표현을 담당할 메시 정의 -> 메쉬는 bp에서 설정.
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	StaticMesh->SetupAttachment(RootComponent);
	
	//발사체 무브먼트 컴포넌트 정의
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->SetUpdatedComponent(SphereComponent);	// 업데이트할 컴포넌트 지정
	ProjectileMovementComponent->InitialSpeed = 1500.0f;				// 초기속도
	ProjectileMovementComponent->MaxSpeed = 1500.0f;					// 최고 속도
	ProjectileMovementComponent->bRotationFollowsVelocity = true;		// 바라보는 방향으로 날리기 설정
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;         // 중력제거
	
	DamageType = UDamageType::StaticClass(); // 대미지 타입 : 기본
	Damage = 10.0f; // 대미지 설정
	
	// 히트 이벤트 등록
	if (GetLocalRole() == ROLE_Authority)
	{
		SphereComponent->OnComponentHit.AddDynamic(this, &ARPCProjectile::OnProjectileImpact);
	}
}

void ARPCProjectile::BeginPlay()
{
	Super::BeginPlay();	
}

void ARPCProjectile::Destroyed()
{
	Super::Destroyed();
	
	FVector spawnLocation = GetActorLocation();
	
	if (ExplosionEffect != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, spawnLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);		
	}    
}

void ARPCProjectile::OnProjectileImpact(UPrimitiveComponent* hitComponent, AActor* otherActor,
	UPrimitiveComponent* otherComp, FVector normalImpulse, const FHitResult& hit)
{
	if (otherActor != nullptr)
	{
		// OtherActor에 대미지 입히고
		UGameplayStatics::ApplyPointDamage(otherActor, Damage, normalImpulse, hit, GetInstigator()->Controller, this, DamageType);
	}
	
	Destroy(); // 파괴 호출
}

void ARPCProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RPCProjectile.generated.h"

UCLASS()
class RPC_TEST_API ARPCProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ARPCProjectile();

protected:
	virtual void BeginPlay() override;

	virtual void Destroyed() override; 
	
	UFUNCTION(Category="Projectile")
	void OnProjectileImpact(UPrimitiveComponent* hitComponent, AActor* otherActor, UPrimitiveComponent* otherComp, FVector normalImpulse, const FHitResult& hit);
	
public:	
	virtual void Tick(float DeltaTime) override;

	// 콜리전 테스트에 사용되는 스피어 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class USphereComponent* SphereComponent;
 
	// 오브젝트의 비주얼 표현을 제공하는 스태틱 메시
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class UStaticMeshComponent* StaticMesh;
 
	// 발사체 움직임을 처리하는 무브먼트 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;
 
	// 발사체가 다른 오브젝트에 영향을 미치고 폭발할 때 사용되는 파티클
	UPROPERTY(EditAnywhere, Category = "Effects")
	class UParticleSystem* ExplosionEffect;
 
	//이 발사체가 가할 대미지 타입과 대미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<class UDamageType> DamageType;
 
	//이 발사체가 가하는 대미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Damage")
	float Damage;
};

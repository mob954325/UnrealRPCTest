#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPCPlayerCharacter.generated.h"

class UInputComponent;

/*
 * RPC 테스트 프로젝트가 사용하는 캐릭터 클래스
 */
UCLASS()
class RPC_TEST_API ARPCPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ARPCPlayerCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void BeginPlay() override;

	// =========================== 체력 리픞리케이트 ===========================
	// 플레이어 최대 체력값
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float maxHealth;
	
	// 플레이어 체력이 0이 되면 사망
	// ReplicatedUsing : 프로퍼티를 네트워크에서 처리할 함수 지정
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
	float currentHealth;
	
	// 현재 체력에 가해진 변경애 대한 RepNotify
	UFUNCTION()
	void OnRep_CurrentHealth();
	
	// 업데이트되는 체력에 반응, 서버에서는 수정 즉시 호출, 클라이언트에서는 RepNotify에 반응하여 호출.
	void OnHealthUpdate();
	
public:	
	virtual void Tick(float DeltaTime) override;	
	
	// 최대 체력 게터
	UFUNCTION(BlueprintPure, Category="Health")
	FORCEINLINE float GetMaxHealth() const { return maxHealth; }
 
	// 현재 체력 게터
	UFUNCTION(BlueprintPure, Category="Health")
	FORCEINLINE float GetCurrentHealth() const { return currentHealth; }
 
	// 현재 체력 세터. 값을 0과 MaxHealth 사이로 범위제한하고 OnHealthUpdate를 호출합니다. 서버에서만 호출되어야 합니다.
	UFUNCTION(BlueprintCallable, Category="Health")
	void SetCurrentHealth(float healthValue);
 
	// 대미지를 받는 이벤트. APawn에서 오버라이드됩니다.
	UFUNCTION(BlueprintCallable, Category = "Health")
	float TakeDamage( float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser ) override;
	
	// =========================== 발사체 관련  ===========================
	UPROPERTY(EditDefaultsOnly, Category = "GamePlay|Projectile")
	TSubclassOf<class ARPCProjectile> projectileClass;
	
	// 발사 딜레이, 단위는 초. 테스트 발사체의 발사 속도를 제어하는 데 사용되지만, 서버 함수의 추가분이 SpawnProjectile을 입력에 직접 바인딩하지 않게 하는 역할도 합니다.
	UPROPERTY(EditDefaultsOnly, Category="Gameplay")
	float FireRate;
 
	// true인 경우 발사체를 발사하는 프로세스 도중입니다. 
	bool bIsFiringWeapon;
 
	// 무기 발사 시작 함수
	UFUNCTION(BlueprintCallable, Category="Gameplay")
	void StartFire();
 
	// 무기 발사 종료 함수. 호출되면 플레이어가 StartFire를 다시 사용할 수 있습니다.
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void StopFire();
 
	// 발사체를 스폰하는 서버 함수
	UFUNCTION(Server, Reliable)
	void HandleFire();
	
	// 스폰 사이에 발사 속도 딜레이를 넣는 타이머 핸들
	FTimerHandle FiringTimer;
};

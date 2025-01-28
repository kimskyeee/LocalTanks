#pragma once

#include "CoreMinimal.h"
#include "AITank.h"
#include "DamageInterface.h"
#include "EShellID.h"
#include "TeamInterface.h"
#include "GameFramework/Pawn.h"
#include "ALightTankCharacter.generated.h"

UCLASS()
class ALightTankCharacter : public AAITank, public IDamageInterface, public ITeamInterface
{
	GENERATED_BODY()
public:
	ALightTankCharacter();

	/// 상태 전이에 필요한 조건 체크 함수들
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	bool IsEnemyInDetectionRange() const;
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	bool IsEnemyInAttackRange() const;
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	bool IsEnemyTooClose() const;
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	bool IsEnemyOutOfRange() const;
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	bool IsDamaged() const;
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	bool IsDestroyed() const;
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	bool IsInCenter();
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	void SetDamaged(bool bCond);

	UFUNCTION(BlueprintCallable, Category = "Tank State")
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
						   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
						   const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Tank State")
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
						 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Tank State")
	void OnOccupiedZoneBeginOverlap();

	UFUNCTION(BlueprintCallable, Category = "Tank State")
	void OnOccupiedZoneEndOverlap();

	UFUNCTION(BlueprintCallable, Category = "Tank State")
	void ClearGoals();
	TQueue<FVector2d>& GetGoalQueue();

	UFUNCTION(BlueprintCallable, Category = "Tank State")
	bool IsInFinalGoal();

	UFUNCTION(BlueprintCallable, Category = "Tank State")
	bool IsReloadedShell();
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	void Fire();
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	FAttackDelegate& GetAttackDelegate() { return AttackDelegate; }
	
	/// Getter, Setter 함수들
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	void SetTurretTargetAngle(float TargetAngle);
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	void SetGunTargetAngle(float TargetAngle);
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	const FTransform& GetGunMuzzleTransfrom() const;
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	class UArrowComponent* GetGunMuzzle() const;
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	class UBoxComponent* GetTurretCollision() const;
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	class UBoxComponent* GetGunCollision() const;
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	float GetGunMinElevationAngle();
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	float GetGunMaxElevationAngle();
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	class UOccupationComponent* GetOccupationComponent() const;
	
	UFUNCTION(BlueprintCallable, Category = "Tank State")
	EShellID GetShellID() const;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/// IDamageInterface 구현
	virtual bool IsPossibleToOccpupy_Implementation() override;
	virtual ETeam GetTeam_Implementation() const override {return ETeam::AI;};
	virtual bool TakeDamage_Implementation(FMyDamageStructure DamageInfo) override;
	// NOT USED
	virtual float GetCurrentHealth_Implementation() override {return HP;}
	virtual float GetMaxHealth_Implementation() override {return MaxHP;};
	virtual bool IsDead_Implementation() override {return HP <= 0;};
	
protected:
	void BindToOccupiedZone();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	void InitDelegate();
	void InitComponents();
	void InitCollisionPreset();
	void InitCamera();
	void InitGroundSensor();
	void InitWidget();
	void SetWidget();
	void ActiveWheelDust();
	void ActiveSkidMark();

	// Goal Queue에 있는 Goal을 하나씩 빼서 처리하는 함수
	void HandleGoal();
	void UpdateGoal();
	void SetNextGoal();
	// 매 틱마다 주어지는 전방 속도와 각 속도를 가지고 이동 및 회전(좌우, 법선 벡터 회전)을 처리하는 함수
	void UpdateMovement(float DeltaTime);
	float GetNormalizedAngleDiff(float CurrentTurretAngle, float TargetTurretAngle);
	void UpdateTurret(float DeltaTime);
	void UpdateGun(float DeltaTime);
	void CalculateTargetSpeed(float DeltaTime);
	void CalculateTargetTurnSpeed(float DeltaTime);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UACLightTankFSM* FSM;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UBoxComponent* ParentBoxCollision;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UBoxComponent* BelowBodyCollision;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UBoxComponent* AboveBodyCollision;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UBoxComponent* GunCollision;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UStaticMeshComponent* TrackLeft;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UStaticMeshComponent* TrackRight;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UStaticMeshComponent* ChassisLeft;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UStaticMeshComponent* ChassisRight;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UStaticMeshComponent* Turret;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UStaticMeshComponent* Hull;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UStaticMeshComponent* Gun;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UArrowComponent* GunMuzzle;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class USpringArmComponent* AITestSpringArm;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UCameraComponent* AITestCamera;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class USCGroundSensor* GroundSensor1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class USCGroundSensor* GroundSensor2;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class USCGroundSensor* GroundSensor3;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class USCGroundSensor* GroundSensor4;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class USCGroundSensor* GroundSensor5;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class USCGroundSensor* GroundSensor6;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class USCGroundSensor* GroundSensor7;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class USCGroundSensor* GroundSensor8;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	TArray<USCGroundSensor*> GroundSensorArray;

	// GroundSensor로부터 받은 정보를 저장하는 변수들
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	FVector AverageNormal;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float AverageHeight;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float HeightOffset = 10.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	int32 CountOnGround;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float DeltaZ;

	// 전방 속도와 가속도를 저장하는 변수들
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float CurrentForwardSpeed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float TargetForwardSpeed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float MaxForwardSpeed = 3000.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float Accel = 50.0f;

	// 회전 속도와 각가속도를 저장하는 변수들
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float AngularVelocity = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float AngularAccelDeg = 270.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float MaxAngularVelocityDeg = 270.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float DeltaThetaDeg;

	// Goal의 위치를 저장하는 변수
	// UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	// UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	FVector2d Goal;
	TQueue<FVector2d> GoalQueue;
	// Goal 설정에 필요한 변수들
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float GoalRadius = 500.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	bool bIsFirstFrame = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	bool bIsGoalSet = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	bool bIsInFinalGoal = false;

	// 상태 전이에 필요한 변수들
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	bool bIsInCenter = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float DetectionRange = 8000.f;
	// TODO : AttackRange는 말 그대로 공격 범위이고, 이 내에서 실질적으로 본인의 공격이 맞을 수 있는지는 검사를 해야 한다.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float AttackRange = 7000.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float RetreatingRange = 5000.f;

	// 공격하기 전에 터렛이 다 정렬 되어야 할까? 아니면 공격을 해야 한다는 정보를 넘기는게 좋을까?
	// 접근 범위에 있는 순간부터 터렛과 배럴을 정렬시키는게 좋을 것 같은데.
	// 그리고 다 정렬되었다면 공격하는 형식이 좋을 것 같다.

	// 터렛의 각도를 조절하는 변수들 : InDegree : Yaw : Left and Right : World Coordinate
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float CurrentTurretAngle; // Yaw
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float TargetTurretAngle; // Yaw
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float CurrentVelocityTurretAngle; // Yaw
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float ConstantVelocityTurretAngle = 39.63; // Yaw

	// 배럴(Gun)의 각도를 조절하는 변수들 : InDegree : Pitch : Up and Down : World Coordinate
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float CurrentGunAngle; // Pitch
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float TargetGunAngle; // Pitch
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float CurrentVelocityGunAngle; // Pitch
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float ConstantGunVelocity = 39.38; // Pitch
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float MaxGunAngle = 20; // Pitch
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float MinGunAngle = -10; // Pitch

	// Armor
	// TODO: 탱크와 Shell의 관계도 생각해보자.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UArmor* Armor;

	// TODO: 관계를 데이터 테이블로 정의할 수 있음 (여기서 복수의 Shell을 사용할 수도 있음 : 이 경우 TArray로 변경)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	EShellID ShellID = EShellID::Pzgr39;

	// 공격을 위한 변수들
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	bool bAvailableToFire = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float ReloadTime = 3.64;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	FTimerHandle ReloadTimerHandle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	FName ShellProfileName = TEXT("Enemy_Missile");

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float HP;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	float MaxHP = 200.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	TSubclassOf<class UUserWidget> HPWidgetClass;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UHPWidget* HPWidget;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UWidgetComponent* HPWidgetComponent;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UMaterial* HPBarMaterial;

	// 데미지 상태 변수
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	bool bDamaged = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UOccupationComponent* OccupationComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UParticleSystem* GunFireVFX;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UParticleSystem* ShockWaveVFX;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UParticleSystem* WheelDustVFX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UParticleSystemComponent* GunFirePSC;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UParticleSystemComponent* ShockWavePSC;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	FAttackDelegate AttackDelegate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	TArray<class UParticleSystemComponent*> WheelDustPSCArray;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	class UNiagaraSystem* WheelSkidNSAsset;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tank Character")
	TArray<class UNiagaraComponent*> WheelSkidNSArray;
};

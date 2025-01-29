#include "ALightTankCharacter.h"

#include "ACLightTankFSM.h"
#include "AShell.h"
#include "FastLogger.h"
#include "HPWidget.h"
#include "MyDamageStructure.h"
#include "NiagaraComponent.h"
#include "OccupationComponent.h"
#include "OccupiedZone.h"
#include "SCGroundSensor.h"
#include "TankGameMode.h"
#include "UArmor.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyProject/Mk/Character/Public/Mk_TankPawn.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraSystem.h"
#include "Particles/ParticleSystemComponent.h"

ALightTankCharacter::ALightTankCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	InitComponents();
	InitWidget();
	InitDelegate();
	InitCollisionPreset();
	InitCamera();
	InitGroundSensor();
}

void ALightTankCharacter::BindToOccupiedZone()
{
	AOccupiedZone* OccupiedZone = Cast<AOccupiedZone>(UGameplayStatics::GetActorOfClass(GetWorld(), AOccupiedZone::StaticClass()));
	if (OccupiedZone)
	{
		OccupiedZone->OnTankBeginOverlapDelegate.AddDynamic(this, &ALightTankCharacter::OnOccupiedZoneBeginOverlap);
		OccupiedZone->OnTankEndOverlapDelegate.AddDynamic(this, &ALightTankCharacter::OnOccupiedZoneEndOverlap);
	}
}

void ALightTankCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetComponents(USCGroundSensor::StaticClass(), GroundSensorArray);
	SetWidget();
	BindToOccupiedZone();
}

void ALightTankCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// TargetGunAngle = 30;
	// TargetTurretAngle = 90;
	HandleGoal();
	CalculateTargetTurnSpeed(DeltaTime);
	CalculateTargetSpeed(DeltaTime);
	UpdateMovement(DeltaTime);
	UpdateTurret(DeltaTime);
	UpdateGun(DeltaTime);
	ActiveWheelDust();
	ActiveSkidMark();
}

bool ALightTankCharacter::IsEnemyInDetectionRange() const
{
	AActor* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (Player)
	{
		FVector PlayerLocation = Player->GetActorLocation();
		FVector MyLocation = GetActorLocation();
		float Distance = FVector::Dist(PlayerLocation, MyLocation);
		if (Distance < DetectionRange)
		{
			return true;
		}
	}
	return false;
}

bool ALightTankCharacter::IsEnemyInAttackRange() const
{
	AActor* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (Player)
	{
		FVector PlayerLocation = Player->GetActorLocation();
		FVector MyLocation = GetActorLocation();
		float Distance = FVector::Dist(PlayerLocation, MyLocation);
		if (Distance < AttackRange)
		{
			return true;
		}
	}
	return false;
}

bool ALightTankCharacter::IsEnemyTooClose() const
{
	AActor* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (Player)
	{
		FVector PlayerLocation = Player->GetActorLocation();
		FVector MyLocation = GetActorLocation();
		float Distance = FVector::Dist(PlayerLocation, MyLocation);
		if (Distance < RetreatingRange)
		{
			return true;
		}
	}
	return false;
}

bool ALightTankCharacter::IsEnemyOutOfRange() const
{
	return !IsEnemyInAttackRange();
}

bool ALightTankCharacter::IsDamaged() const
{
	return bDamaged;
}

bool ALightTankCharacter::IsDestroyed() const
{
	if (this->HP <= 0)
	{
		return true;
	}
	return false;
}

bool ALightTankCharacter::IsInCenter()
{
	// TODO: 중앙에 있을 때 해당 변수가 바뀌어져야 함.
	// Overalap으로 해결 혹은 Radis로 해결
	return bIsInCenter;
}

void ALightTankCharacter::SetDamaged(bool bCond)
{
	bDamaged = bCond;
}

void ALightTankCharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void ALightTankCharacter::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void ALightTankCharacter::OnOccupiedZoneBeginOverlap()
{
	bIsInCenter = true;
}

void ALightTankCharacter::OnOccupiedZoneEndOverlap()
{
	bIsInCenter = false;
}

void ALightTankCharacter::ClearGoals()
{
	GoalQueue.Empty();
	bIsGoalSet = false;
	bIsInFinalGoal = false;
}

TQueue<FVector2d>& ALightTankCharacter::GetGoalQueue()
{
	return GoalQueue;
}

bool ALightTankCharacter::IsInFinalGoal()
{
	return bIsInFinalGoal;
}

void ALightTankCharacter::SetTurretTargetAngle(float TargetAngle)
{
	TargetTurretAngle = TargetAngle;
}

void ALightTankCharacter::SetGunTargetAngle(float TargetAngle)
{
	TargetGunAngle = TargetAngle;
}

const FTransform& ALightTankCharacter::GetGunMuzzleTransfrom() const
{
	return GunMuzzle->GetComponentTransform();
}

UArrowComponent* ALightTankCharacter::GetGunMuzzle() const
{
	return GunMuzzle;
}

class UBoxComponent* ALightTankCharacter::GetTurretCollision() const
{
	return AboveBodyCollision;
}

class UBoxComponent* ALightTankCharacter::GetGunCollision() const
{
	return GunCollision;
}

float ALightTankCharacter::GetGunMinElevationAngle()
{
	return MinGunAngle;
}

float ALightTankCharacter::GetGunMaxElevationAngle()
{
	return MaxGunAngle;
}

class UOccupationComponent* ALightTankCharacter::GetOccupationComponent() const
{
	return OccupationComponent;
}

EShellID ALightTankCharacter::GetShellID() const
{
	return ShellID;
}

void ALightTankCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
}

bool ALightTankCharacter::IsReloadedShell()
{
	return bAvailableToFire;
}

void ALightTankCharacter::Fire()
{
	bAvailableToFire = false;
	TWeakObjectPtr<ALightTankCharacter> WeakThis = this;
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, FTimerDelegate::CreateLambda([WeakThis]()
	{
		ALightTankCharacter* StrongThis = WeakThis.Get();
		if (StrongThis)
		{
			StrongThis->bAvailableToFire = true;
		}
	}), ReloadTime, false);
	FVector ShellLocation = GunMuzzle->GetComponentLocation();
	// ShellLocation += GunMuzzle->GetForwardVector() * 650;
	Armor->FireShell(ShellID, ShellLocation, GunMuzzle->GetComponentRotation(), ShellProfileName, AMk_TankPawn::StaticClass());
	AttackDelegate.OnAttackDelegate.Broadcast();
	GunFirePSC->Activate(true);
}

bool ALightTankCharacter::IsPossibleToOccpupy_Implementation()
{
	return OccupationComponent->IsOccupyingAvailable();
}

bool ALightTankCharacter::TakeDamage_Implementation(FMyDamageStructure DamageInfo)
{
	float ActualDamage = DamageInfo.ShellBasicInfo.ShellDamage;
	HP -= ActualDamage;
	HP = FMath::Clamp(HP, 0.0f, MaxHP);
	
	// Widget Update
	HPWidget->UpdateHPProgessBar(this->HP, this->MaxHP);

	if (HP > 0)
	{
		bDamaged = true;
	}
	
	return bDamaged;
}

void ALightTankCharacter::InitDelegate()
{
	ParentBoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ALightTankCharacter::OnBeginOverlap);
	ParentBoxCollision->OnComponentEndOverlap.AddDynamic(this, &ALightTankCharacter::OnEndOverlap);
}

void ALightTankCharacter::InitComponents()
{
	FSM = CreateDefaultSubobject<UACLightTankFSM>(TEXT("FSM"));
	
	ParentBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ParentBoxCollision"));
	ParentBoxCollision->SetMobility(EComponentMobility::Movable);
	RootComponent = ParentBoxCollision;

	BelowBodyCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BelowBodyCollision"));
	BelowBodyCollision->SetupAttachment(ParentBoxCollision);
	BelowBodyCollision->SetRelativeLocation({0, 0, 77});
	BelowBodyCollision->SetRelativeRotation({0, -90, 0});
	BelowBodyCollision->SetBoxExtent({130.000000, 250.000000, 80.000000});

	AboveBodyCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("AboveBodyCollision"));
	AboveBodyCollision->SetupAttachment(ParentBoxCollision);
	AboveBodyCollision->SetRelativeLocation({0, 0, 180});
	AboveBodyCollision->SetRelativeRotation({0, 0, 0});
	AboveBodyCollision->SetBoxExtent({100, 80, 25});

	GunCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("GunCollision"));
	GunCollision->SetupAttachment(AboveBodyCollision);
	GunCollision->SetRelativeLocation({70, 10, -20});
	GunCollision->SetRelativeRotation({0, 0, 0});
	GunCollision->SetBoxExtent({10, 10, 10});
	
	TrackLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrackLeft"));
	TrackLeft->SetupAttachment(BelowBodyCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> TrackLeftMesh
	(TEXT("/Game/LightTank/Character/Models/german_G139_MKA_normal_l_Chassis_pri_0_4.german_G139_MKA_normal_l_Chassis_pri_0_4"));
	if (TrackLeftMesh.Succeeded())
	{
		TrackLeft->SetStaticMesh(TrackLeftMesh.Object);
	}
	TrackLeft->SetRelativeLocation({0, 0, -77});
	
	TrackRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrackRight"));
	TrackRight->SetupAttachment(BelowBodyCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> TrackRightMesh
	(TEXT("/Game/LightTank/Character/Models/german_G139_MKA_normal_l_Chassis_pri_0_3.german_G139_MKA_normal_l_Chassis_pri_0_3"));
	if (TrackRightMesh.Succeeded())
	{
		TrackRight->SetStaticMesh(TrackRightMesh.Object);
	}
	TrackRight->SetRelativeLocation({0, 0, -77});
	
	ChassisLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChassisLeft"));
	ChassisLeft->SetupAttachment(BelowBodyCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ChassisLeftMesh
	(TEXT("/Game/LightTank/Character/Models/german_G139_MKA_normal_l_Chassis_pri_0_2.german_G139_MKA_normal_l_Chassis_pri_0_2"));
	if (ChassisLeftMesh.Succeeded())
	{
		ChassisLeft->SetStaticMesh(ChassisLeftMesh.Object);
	}
	ChassisLeft->SetRelativeLocation({0, 0, -77});
	
	ChassisRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChassisRight"));
	ChassisRight->SetupAttachment(BelowBodyCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ChassisRightMesh
	(TEXT("/Game/LightTank/Character/Models/german_G139_MKA_normal_l_Chassis_pri_0_1.german_G139_MKA_normal_l_Chassis_pri_0_1"));
	if (ChassisRightMesh.Succeeded())
	{
		ChassisRight->SetStaticMesh(ChassisRightMesh.Object);
	}
	ChassisRight->SetRelativeLocation({0, 0, -77});
	
	Hull = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hull"));
	Hull->SetupAttachment(BelowBodyCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> HullMesh
	(TEXT("/Game/LightTank/Character/Models/german_G139_MKA_normal_l_Hull_pri_0_5.german_G139_MKA_normal_l_Hull_pri_0_5"));
	if (HullMesh.Succeeded())
	{
		Hull->SetStaticMesh(HullMesh.Object);
	}
	Hull->SetRelativeLocation({0, 0, -77});
	
	Turret = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret"));
	Turret->SetupAttachment(AboveBodyCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> TurretMesh
	(TEXT("/Game/LightTank/Character/Models/german_G139_MKA_normal_l_Turret_01_pri_0_6.german_G139_MKA_normal_l_Turret_01_pri_0_6"));
	if (TurretMesh.Succeeded())
	{
		Turret->SetStaticMesh(TurretMesh.Object);
	}
	Turret->SetRelativeLocation({0, 10, -180});
	Turret->SetRelativeRotation({0, -90, 0});
	
	Gun = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gun"));
	Gun->SetupAttachment(GunCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GunMesh
	(TEXT("/Game/LightTank/Character/Models/german_G139_MKA_normal_l_Gun_01_pri_0_7.german_G139_MKA_normal_l_Gun_01_pri_0_7"));
	if (GunMesh.Succeeded())
	{
		Gun->SetStaticMesh(GunMesh.Object);
	}
	Gun->SetRelativeLocation({-70, 0, -160});
	Gun->SetRelativeRotation({0, -90, 0});

	GunMuzzle = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	GunMuzzle->SetupAttachment(GunCollision);
	GunMuzzle->SetRelativeLocation({167, 0, 12});

	Armor = CreateDefaultSubobject<UArmor>(TEXT("Armor_LightTank"));

	OccupationComponent = CreateDefaultSubobject<UOccupationComponent>(TEXT("OccupationComponent"));

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_GunFireVFX
	(TEXT("/Game/VigilanteContent/Vehicles/West_Tank_M1A1Abrams/FX/PS_MuzzleFire_01_M1A1Abrams.PS_MuzzleFire_01_M1A1Abrams"));
	if (P_GunFireVFX.Succeeded())
	{
		GunFireVFX = P_GunFireVFX.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_ShockWaveVFX
	(TEXT("/Game/VigilanteContent/Vehicles/West_Tank_M1A1Abrams/FX/PS_ShockWave_M1A1Abrams.PS_ShockWave_M1A1Abrams"));
	if (P_ShockWaveVFX.Succeeded())
	{
		ShockWaveVFX = P_ShockWaveVFX.Object;
	}

	GunFirePSC = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("GunFirePSC"));
	GunFirePSC->SetupAttachment(GunMuzzle);
	GunFirePSC->SetRelativeLocation({0, 0, 0});
	GunFirePSC->SetRelativeRotation({0, 0, 0});
	GunFirePSC->SetTemplate(GunFireVFX);
	GunFirePSC->bAutoActivate = false;

	ShockWavePSC = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ShockWavePSC"));
	ShockWavePSC->SetupAttachment(GunMuzzle);
	ShockWavePSC->SetRelativeLocation({0, 0, 0});
	ShockWavePSC->SetRelativeRotation({0, 0, 0});
	ShockWavePSC->SetWorldScale3D({0.5, 0.5, 0.5});
	ShockWavePSC->SetTemplate(ShockWaveVFX);
	ShockWavePSC->bAutoActivate = false;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_WheelDustVFX
	(TEXT("/Game/VigilanteContent/Vehicles/West_Tank_M1A1Abrams/FX/PS_Dust_WheelTrack_M1A1Abrams.PS_Dust_WheelTrack_M1A1Abrams"));
	if (P_WheelDustVFX.Succeeded())
	{
		WheelDustVFX = P_WheelDustVFX.Object;
	}

	for (int32 i = 0; i < 4; i++)
	{
		UParticleSystemComponent* WheelDustPSC = CreateDefaultSubobject<UParticleSystemComponent>(FName(*FString::Printf(TEXT("WheelDustPSC_%d"), i)));
		WheelDustPSC->SetupAttachment(BelowBodyCollision);
		WheelDustPSC->SetTemplate(WheelDustVFX);
		WheelDustPSC->bAutoActivate = false;
		WheelDustPSCArray.Add(WheelDustPSC);
	}
	// WheelDustPSCArray
	WheelDustPSCArray[0]->SetRelativeLocation({100, 120, -70});
	WheelDustPSCArray[1]->SetRelativeLocation({-100, 120, -70});
	WheelDustPSCArray[2]->SetRelativeLocation({100, -120, -70});
	WheelDustPSCArray[3]->SetRelativeLocation({-100, -120, -70});
	WheelDustPSCArray[0]->SetRelativeRotation({0, 90, 0});
	WheelDustPSCArray[1]->SetRelativeRotation({0, 90, 0});
	WheelDustPSCArray[2]->SetRelativeRotation({0, 90, 0});
	WheelDustPSCArray[3]->SetRelativeRotation({0, 90, 0});

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NS_WheelSkidAsset
	(TEXT("/Game/Material/TankSkid/NS_SkidMark.NS_SkidMark"));
	if (NS_WheelSkidAsset.Succeeded())
	{
		WheelSkidNSAsset = NS_WheelSkidAsset.Object;
	}
	
	for (int32 i = 0; i < 4; i++)
	{
		UNiagaraComponent* WheelSkidNS = CreateDefaultSubobject<UNiagaraComponent>(FName(*FString::Printf(TEXT("WheelSkidNS_%d"), i)));
		WheelSkidNS->SetupAttachment(BelowBodyCollision);
		WheelSkidNS->SetAutoActivate(false);
		WheelSkidNS->SetAsset(WheelSkidNSAsset);
		WheelSkidNSArray.Add(WheelSkidNS);
	}
	WheelSkidNSArray[0]->SetRelativeLocation({100, 120, -70});
	WheelSkidNSArray[1]->SetRelativeLocation({-100, 120, -70});
	WheelSkidNSArray[2]->SetRelativeLocation({100, -120, -70});
	WheelSkidNSArray[3]->SetRelativeLocation({-100, -120, -70});
	WheelSkidNSArray[0]->SetRelativeRotation({0, 90, 0});
	WheelSkidNSArray[1]->SetRelativeRotation({0, 90, 0});
	WheelSkidNSArray[2]->SetRelativeRotation({0, 90, 0});
	WheelSkidNSArray[3]->SetRelativeRotation({0, 90, 0});
}

void ALightTankCharacter::InitCollisionPreset()
{
	// Use Collision of Static Mesh Component
	ParentBoxCollision->SetCollisionProfileName(TEXT("Enemy_Tank"));
	BelowBodyCollision->SetCollisionProfileName(TEXT("Enemy_Tank"));
	AboveBodyCollision->SetCollisionProfileName(TEXT("Enemy_Tank"));
	GunCollision->SetCollisionProfileName(TEXT("Enemy_Tank"));
	HPWidgetComponent->SetCollisionProfileName(TEXT("Enemy_Tank"));

	ParentBoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	BelowBodyCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	AboveBodyCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	GunCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	HPWidgetComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

	TrackLeft->SetCollisionProfileName(TEXT("Enemy_Tank"));
	TrackRight->SetCollisionProfileName(TEXT("Enemy_Tank"));
	ChassisLeft->SetCollisionProfileName(TEXT("Enemy_Tank"));
	ChassisRight->SetCollisionProfileName(TEXT("Enemy_Tank"));
	Turret->SetCollisionProfileName(TEXT("Enemy_Tank"));
	Hull->SetCollisionProfileName(TEXT("Enemy_Tank"));
	Gun->SetCollisionProfileName(TEXT("Enemy_Tank"));
}

void ALightTankCharacter::InitCamera()
{
	AITestSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("AITestSpringArm"));
	AITestSpringArm->SetupAttachment(RootComponent);
	AITestSpringArm->SetRelativeLocation({0, 0, 0});
	AITestSpringArm->SetRelativeRotation({0, 0, 0});
	AITestSpringArm->SocketOffset = {0, 0, 250};
	AITestSpringArm->TargetArmLength = 650;
	AITestSpringArm->bDoCollisionTest = false;
	AITestSpringArm->bEnableCameraLag = true;
	AITestSpringArm->bEnableCameraRotationLag = true;
	AITestSpringArm->CameraLagSpeed = 3;
	AITestSpringArm->CameraRotationLagSpeed = 3;

	AITestCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("AITestCamera"));
	AITestCamera->SetupAttachment(AITestSpringArm, USpringArmComponent::SocketName);
	AITestCamera->bUsePawnControlRotation = false;
}

void ALightTankCharacter::InitGroundSensor()
{
	GroundSensor1 = CreateDefaultSubobject<USCGroundSensor>(TEXT("GroundSensor1"));
	GroundSensor1->SetupAttachment(RootComponent);
	GroundSensor1->SetRelativeLocation({-100, 100, 30});
	GroundSensor1->SetRelativeRotation({0, 0, 0});

	GroundSensor2 = CreateDefaultSubobject<USCGroundSensor>(TEXT("GroundSensor2"));
	GroundSensor2->SetupAttachment(RootComponent);
	GroundSensor2->SetRelativeLocation({100, 100, 30});
	GroundSensor2->SetRelativeRotation({0, 0, 0});
	
	GroundSensor3 = CreateDefaultSubobject<USCGroundSensor>(TEXT("GroundSensor3"));
	GroundSensor3->SetupAttachment(RootComponent);
	GroundSensor3->SetRelativeLocation({100, -100, 30});
	GroundSensor3->SetRelativeRotation({0, 0, 0});
	
	GroundSensor4 = CreateDefaultSubobject<USCGroundSensor>(TEXT("GroundSensor4"));
	GroundSensor4->SetupAttachment(RootComponent);
	GroundSensor4->SetRelativeLocation({-100, -100, 30});
	GroundSensor4->SetRelativeRotation({0, 0, 0});
	
	GroundSensor5 = CreateDefaultSubobject<USCGroundSensor>(TEXT("GroundSensor5"));
	GroundSensor5->SetupAttachment(RootComponent);
	GroundSensor5->SetRelativeLocation({50, 100, 30});
	GroundSensor5->SetRelativeRotation({0, 0, 0});
	
	GroundSensor6 = CreateDefaultSubobject<USCGroundSensor>(TEXT("GroundSensor6"));
	GroundSensor6->SetupAttachment(RootComponent);
	GroundSensor6->SetRelativeLocation({50, -100, 30});
	GroundSensor6->SetRelativeRotation({0, 0, 0});
	
	GroundSensor7 = CreateDefaultSubobject<USCGroundSensor>(TEXT("GroundSensor7"));
	GroundSensor7->SetupAttachment(RootComponent);
	GroundSensor7->SetRelativeLocation({-50, 100, 30});
	GroundSensor7->SetRelativeRotation({0, 0, 0});
	
	GroundSensor8 = CreateDefaultSubobject<USCGroundSensor>(TEXT("GroundSensor8"));
	GroundSensor8->SetupAttachment(RootComponent);
	GroundSensor8->SetRelativeLocation({-50, -100, 30});
	GroundSensor8->SetRelativeRotation({0, 0, 0});
}

void ALightTankCharacter::InitWidget()
{
	HPWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPWidgetComponent"));
	HPWidgetComponent->SetupAttachment(ParentBoxCollision);
	HPWidgetComponent->SetRelativeLocation({0, 0, 250});

	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_HPWidget
	(TEXT("/Game/NewSkye/PlayerUI/WBP_HealthBar.WBP_HealthBar_C"));
	if (WBP_HPWidget.Succeeded())
	{
		HPWidgetClass = WBP_HPWidget.Class;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> M_HPBar(
	TEXT("/Game/NewSkye/PlayerUI/Widget3DPassThrough_twoside.Widget3DPassThrough_twoside"));
	if (M_HPBar.Succeeded())
	{
		HPBarMaterial = M_HPBar.Object;
	}
}

void ALightTankCharacter::SetWidget()
{
	HP = MaxHP;
	HPWidget = CreateWidget<UHPWidget>(GetWorld(), HPWidgetClass);
	if (HPWidget)
	{
		HPWidgetComponent->SetWidget(Cast<UUserWidget>(HPWidget));
		HPWidget->UpdateHPProgessBar(HP, MaxHP);
		HPWidget->SetName(TEXT("DeuleeTank"));
		if (HPBarMaterial)
		{
			HPWidgetComponent->SetMaterial(0, HPBarMaterial);
		}
	}
}

void ALightTankCharacter::ActiveWheelDust()
{
	// CurrentForwardSpeed에 따라 WheelDustPSCArray의 활성화 여부를 결정 (속도가 빠를수록 활성화)
	float SpeedRatio = FMath::Clamp(FMath::Abs(CurrentForwardSpeed) / MaxForwardSpeed, 0.0f, 1.0f);

	constexpr float MaxDustScale = 4.0f;
	float DustIntensity = SpeedRatio * MaxDustScale;

	for (auto WheelDustPSC : WheelDustPSCArray)
	{
		// 우선 파티클이 꺼져 있을 수 있으므로 활성화
		if (WheelDustPSC && !WheelDustPSC->IsActive())
		{
			WheelDustPSC->Activate(true);
		}
    
		// 만약 파티클 자체를 스케일링하고 싶다면 (파티클 시스템의 전체 크기):
		WheelDustPSC->SetWorldScale3D(FVector(DustIntensity));
	}
}

void ALightTankCharacter::ActiveSkidMark()
{
	float SpeedRatio = FMath::Clamp(FMath::Abs(CurrentForwardSpeed) / MaxForwardSpeed, 0.0f, 1.0f);

	if (SpeedRatio < 0.01f)
	{
		for (auto WheelSkidNS : WheelSkidNSArray)
		{
			if (WheelSkidNS && WheelSkidNS->IsActive())
			{
				WheelSkidNS->Deactivate();
			}
		}
		return;
	}
	
	for (auto WheelSkidNS : WheelSkidNSArray)
	{
		if (WheelSkidNS && !WheelSkidNS->IsActive())
		{
			WheelSkidNS->Activate(true);
		}
	}
}

void ALightTankCharacter::HandleGoal()
{
	if (!bIsGoalSet)
	{
		SetNextGoal();
	}
	else
	{
		UpdateGoal();
	}
}

void ALightTankCharacter::UpdateGoal()
{
	float DY = Goal.Y - GetActorLocation().Y;
	float DX = Goal.X - GetActorLocation().X;
	float Distance = FMath::Sqrt(FMath::Square(DY) + FMath::Square(DX));

	if (Distance < GoalRadius)
	{
		SetNextGoal();
	}
}

void ALightTankCharacter::SetNextGoal()
{
	if (GoalQueue.Dequeue(Goal))
	{
		bIsGoalSet = true;
		bIsInFinalGoal = false;
	}
	else
	{
		bIsGoalSet = false;
		bIsInFinalGoal = true;
	}
}

// 외부에서 설정되어야 하는 값들
// TargetForwardSpeed : 목표 전방 속도
// TargetTurnSpeed : 목표 회전 속도 (좌 우 회전 속도)
void ALightTankCharacter::UpdateMovement(float DeltaTime)
{
	// 값 초기화
	AverageNormal = FVector::ZeroVector;
	AverageHeight = 0;
	CountOnGround = 0;

	// 땅 센서들의 정보를 이용하여 평균 법선 벡터와 평균 높이를 구함
	for (auto GroundSensor : GroundSensorArray)
	{
		AverageNormal += GroundSensor->GetHitNormal();
		AverageHeight += GroundSensor->GetHitLocation().Z;
		if (GroundSensor->IsOnGround())
		{
			CountOnGround++;
		}
	}

	// 땅에 닿은 센서가 하나 이상일 때만 평균을 구함
	if (CountOnGround != 0)
	{
		AverageNormal /= CountOnGround;
		AverageHeight /= CountOnGround;
	}
	else
	{
		AverageNormal = FVector::UpVector;
		AverageHeight = GetActorLocation().Z;
	}

	// 물리 기반
	float InterpolatedForwardSpeed = CurrentForwardSpeed;
	if (CurrentForwardSpeed < TargetForwardSpeed)
    {
        InterpolatedForwardSpeed += Accel * DeltaTime;
		InterpolatedForwardSpeed = FMath::Min(InterpolatedForwardSpeed, TargetForwardSpeed);
    }
    else if (CurrentForwardSpeed > TargetForwardSpeed)
    {
        InterpolatedForwardSpeed -= Accel * DeltaTime;
    	InterpolatedForwardSpeed = FMath::Max(InterpolatedForwardSpeed, TargetForwardSpeed);
    }
	
	// 높이 보정
	DeltaZ = (AverageHeight + HeightOffset) - GetActorLocation().Z;

	// P = P0 + V * T 로 최종 위치값을 구함, (여기서 DeltaZ는 높이값)
	FVector WorldOffset = GetActorForwardVector() * InterpolatedForwardSpeed * GetWorld()->GetDeltaSeconds();

	// 좌우 회전값을 구함 (TargetTurnSpeed : 목표 각속도 : radians로 계산됨)
	float TargetTurnSpeedInDegree = FMath::RadiansToDegrees(AngularVelocity);
	FRotator TurnRotation = FRotator({0, TargetTurnSpeedInDegree * GetWorld()->GetDeltaSeconds(), 0});
	// 땅 감지 센서를 통해 구한 탱크가 표면에 붙었을 경우 표면의 평균 법선 벡터(탱크의 UP Vector가 됨 : Z)와 탱크의 오른쪽 벡터(탱크의 Right Vector : Y)를 이용하여 회전 행렬을 구한 뒤 회전 값을 구함.
	FRotator GroundRotation = FRotationMatrix::MakeFromZY(AverageNormal, GetActorRightVector()).Rotator();

	// 더하는 순서 중요!
	FRotator CombineRotation = TurnRotation + GroundRotation;
	
	// 위 둘의 회전 값과 현재 회전 값을 보간하여 회전값을 구한 뒤 적용
	FRotator InterpolatedRotation = FMath::RInterpTo(GetActorRotation(), CombineRotation, GetWorld()->GetDeltaSeconds(), 2.0f);
	SetActorRotation(InterpolatedRotation);

	// 회전 후 DeltaZ를 다시 구함
	WorldOffset += GetActorUpVector() * DeltaZ;

	// Legacy Code : 딱딱하게 회전함
	// SetActorRotation(FRotationMatrix::MakeFromZY(AverageNormal, GetActorRightVector()).Rotator());

	// 위치값 적용
	AddActorWorldOffset(WorldOffset);

	// 현재 전방 속도를 저장 (이로 인하여 점점 빨라짐, 선형 보간 법칙에 의하여)
	CurrentForwardSpeed = InterpolatedForwardSpeed;
}

float ALightTankCharacter::GetNormalizedAngleDiff(float InCurrentTurretAngle, float InTargetTurretAngle)
{
	// 먼저 현재 각도와 목표 각도를 -180 ~ +180 사이로 정규화
	float NormalizedCurrent = UKismetMathLibrary::NormalizeAxis(InCurrentTurretAngle);
	float NormalizedTarget  = UKismetMathLibrary::NormalizeAxis(InTargetTurretAngle);

	// 차이를 구한 뒤, 다시 -180 ~ +180 사이로 정규화
	float Delta = NormalizedTarget - NormalizedCurrent;
	Delta = UKismetMathLibrary::NormalizeAxis(Delta);

	return Delta;
}

void ALightTankCharacter::UpdateTurret(float DeltaTime)
{
	// 현재 터렛(포신) 각도 (Yaw 기준)
	FRotator CurrentRotation = AboveBodyCollision->GetRelativeRotation();
	CurrentTurretAngle = CurrentRotation.Yaw;
	CurrentTurretAngle = UKismetMathLibrary::NormalizeAxis(CurrentTurretAngle);
	TargetTurretAngle = UKismetMathLibrary::NormalizeAxis(TargetTurretAngle);

	// 목표 각도와의 '가장 짧은 쪽' 차이를 구한다
	float DeltaTurretAngle = GetNormalizedAngleDiff(CurrentTurretAngle, TargetTurretAngle);

	// 정규화로 인한 반대 회전 방지
	float NewTargetTurretAngle = CurrentTurretAngle + DeltaTurretAngle;

	// - : 시계 방향, + : 반시계 방향
	float DeltaSign = FMath::Sign(DeltaTurretAngle);

	// P = P0 + V*t
	float InterpolatedTurretAngle = CurrentTurretAngle + DeltaSign * ConstantVelocityTurretAngle * DeltaTime;

	// 보간 결과가 최종 목표를 overshoot 했는지 검사
	//    (DeltaTurretAngle 부호와, InterpolatedTurretAngle와 TargetTurretAngle 차이의 부호가 다르면 넘어섰다는 뜻)
	float NextDiff = GetNormalizedAngleDiff(InterpolatedTurretAngle, TargetTurretAngle);
	if (FMath::Sign(DeltaTurretAngle) != FMath::Sign(NextDiff))
	{
		InterpolatedTurretAngle = NewTargetTurretAngle;
	}

	// 실제 회전 적용
	FRotator NewRotation(0.0f, InterpolatedTurretAngle, 0.0f);
	AboveBodyCollision->SetRelativeRotation(NewRotation);
}

void ALightTankCharacter::UpdateGun(float DeltaTime)
{
	// TargetGunAngle : 목표 총포 각도
	// 로 회전해야 함
	FRotator CurrentRotation = GunCollision->GetRelativeRotation(); // Local
	CurrentGunAngle = CurrentRotation.Pitch;
	CurrentGunAngle = UKismetMathLibrary::NormalizeAxis(CurrentGunAngle);
	TargetGunAngle = UKismetMathLibrary::NormalizeAxis(TargetGunAngle);

	float DeltaGunAngle = GetNormalizedAngleDiff(CurrentGunAngle, TargetGunAngle);

	
	// 정규화로 인한 반대 회전 방지
	float NewTargetGunAngle = CurrentGunAngle + DeltaGunAngle;
	
	float DeltaSign = FMath::Sign(DeltaGunAngle);

	// P = P0 + V*t
	float InterpolatedGunAngle = CurrentGunAngle + DeltaSign * ConstantGunVelocity * DeltaTime;
	
	// 보간 결과가 최종 목표를 overshoot 했는지 검사
	float NextDiff = GetNormalizedAngleDiff(InterpolatedGunAngle, TargetGunAngle);
	if (FMath::Sign(DeltaGunAngle) != FMath::Sign(NextDiff))
	{
		InterpolatedGunAngle = NewTargetGunAngle;
	}

	// 최대 최소 각도 제한
	InterpolatedGunAngle = FMath::Clamp(InterpolatedGunAngle, MinGunAngle, MaxGunAngle);
	
	FRotator NewRotation = FRotator(InterpolatedGunAngle, 0.0f, 0.0f);
	GunCollision->SetRelativeRotation(NewRotation);
}

void ALightTankCharacter::CalculateTargetSpeed(float DeltaTime)
{
	// 우선 목표 지점과 액터의 위치를 뺀 뒤 거리를 구함
	float DY = Goal.Y - GetActorLocation().Y;
	float DX = Goal.X - GetActorLocation().X;

	float Distance = FMath::Sqrt(FMath::Square(DY) + FMath::Square(DX));
	const float ThresholdDeg = 20.f;
	if (Distance < 2000 && FMath::Abs(DeltaThetaDeg) > ThresholdDeg)
	{
		TargetForwardSpeed = 50;
	}
	else if (Distance < 4000 && FMath::Abs(DeltaThetaDeg) > ThresholdDeg + 20)
	{
		TargetForwardSpeed = 150;
	}
	else
	{
		TargetForwardSpeed = 300;
	}
	//
	// 목표 지점에 도착하면 출력
	if (bIsInFinalGoal)
	{
		TargetForwardSpeed = 0;
		AngularVelocity = 0;
	}
}

void ALightTankCharacter::CalculateTargetTurnSpeed(float DeltaTime)
{
	// (1) 목표 지점에서 현재의 위치를 빼서 방향을 구함
	float Dx = Goal.X - GetActorLocation().X;
	float Dy = Goal.Y - GetActorLocation().Y;
	float GoalHeadingAngle = FMath::Atan2(Dy, Dx); // Radians

	// (2) 현재의 각도와 목표 각도의 차이 ( -PI ~ PI )
	float Theta = FMath::DegreesToRadians(GetActorRotation().Yaw); // Radians
	float DeltaTheta = GoalHeadingAngle - Theta;
	DeltaTheta = FMath::Atan2(FMath::Sin(DeltaTheta), FMath::Cos(DeltaTheta));

	// (3) 일정한 회전 속도를 적용할 때
	//     예) DeltaTheta가 +면 반시계(또는 시계)로 일정 속도, -면 그 반대 방향
	//     다만, 일정 범위(Threshold) 이하면 멈춘다.
    
	// 목표 각도에 충분히 근접했는지 검사 (예: 5도 이내)
	const float AngleThresholdDeg = 3.f; 
	DeltaThetaDeg = FMath::RadiansToDegrees(DeltaTheta);

	float AngularVelocityDeg = FMath::RadiansToDegrees(AngularVelocity);
	// (4) 회전 속도 결정
	if (FMath::Abs(DeltaThetaDeg) > AngleThresholdDeg)
	{
		// DeltaThetaDeg가 양수면 한 방향, 음수면 반대 방향
		// sign(DeltaThetaDeg)가 +1 / -1
		float SignOfDelta = (DeltaThetaDeg > 0.f) ? +1.f : -1.f;
		float DesiredAngluarVelocity = SignOfDelta * MaxAngularVelocityDeg;

		float VelocityDiff = DesiredAngluarVelocity - AngularVelocityDeg;

		// 각가속도 * DeltaTime 만큼 변화
		float AccelStep = AngularAccelDeg * DeltaTime;

		if (FMath::Abs(VelocityDiff) > KINDA_SMALL_NUMBER)
		{
			if (VelocityDiff > 0.f)
			{
				// 아직 목표속도보다 작으면 가속
				AngularVelocityDeg += AccelStep;
				// 목표속도를 초과하지 않도록 Clamp
				AngularVelocityDeg = FMath::Min(AngularVelocityDeg, DesiredAngluarVelocity);
			}
			else
			{
				// 목표속도보다 크면 (부호 반대 방향으로) 감속
				AngularVelocityDeg -= AccelStep;
				// 목표속도보다 더 작아지지 않도록 Clamp
				AngularVelocityDeg = FMath::Max(AngularVelocityDeg, DesiredAngluarVelocity);
			}
		}
	}
	else
	{
		// 목표 각도 근처면 회전 정지
		AngularVelocityDeg = 0.f;
	}

	// (5) 최종: 도/초를 언리얼의 Yaw 속도( deg/sec )로 사용한다면
	//     Project Setting이나 캐릭터 이동 로직에 따라 단위 처리를 맞춰야 함.
	AngularVelocity = FMath::DegreesToRadians(AngularVelocityDeg);
}

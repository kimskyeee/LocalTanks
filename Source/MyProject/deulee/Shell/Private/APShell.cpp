#include "APShell.h"

#include "FastLogger.h"
#include "ShellDecalComponent.h"
#include "UArmor.h"
#include "Components/BoxComponent.h"
#include "MyProject/Mk/Character/Public/Mk_TankPawn.h"

APShell::APShell()
{
	PrimaryActorTick.bCanEverTick = true;

	Collision->OnComponentBeginOverlap.AddDynamic(this, &APShell::OnShellOverlapEvent);

	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void APShell::BeginPlay()
{
	Super::BeginPlay();

	DeActive();
}

void APShell::Tick(float DeltaTime)
{
	// 부모의 기본 포물선 운동을 따름
	Super::Tick(DeltaTime);
}

void APShell::OnShellOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// TODO: Add Shell ID Enum
	FFastLogger::LogScreen(FColor::Red, TEXT("Shell Overlap! : %s: %s"), *OtherActor->GetName(), *OtherComp->GetName());
	WrapShellDamageInfo();

	// TODO: Player면 공격 인터페이스 호출
	AMk_TankPawn* TankPawn = Cast<AMk_TankPawn>(OtherActor);
	if (TankPawn)
	{
		TankPawn->TakeDamage_Implementation(DamageInfo);
		ShellDecalComponent->SpawnDecal(SweepResult, true);
	}
	else
	{
		ShellDecalComponent->SpawnDecal(SweepResult, true);
	}
	
	DeActive();
	Armor->ReleaseShell(ShellInfo.ShellID, this);

	FFastLogger::LogScreen(FColor::Red, TEXT("Shell Overlap!, Sweep Position: %s"), *SweepResult.ImpactPoint.ToString());
	// Debug Draw
	DrawDebugSphere(GetWorld(), SweepResult.ImpactPoint, 10.0f, 12, FColor::Red, false, 2.0f);
}

void APShell::WrapShellDamageInfo()
{
	// ShellDamageEvent.ImpactLocation = GetActorLocation();
	// ShellDamageEvent.ImpactNormal = GetActorForwardVector() * -1; // 방향이 반대로 들어가야 함
	// ShellDamageEvent.ShellID = ShellInfo.ShellID;
	// ShellDamageEvent.ShellInfo = ShellInfo;

	// TODO: Beta: 이 부분에 이제 공격을 받았을 때 필요한 로직을 적어야 함.
	DamageInfo.DamageLocation = GetActorLocation();
	DamageInfo.ImpactNormal = GetActorForwardVector() * -1;
	DamageInfo.ShellBasicInfo = ShellInfo;
	DamageInfo.bAbove = false;
	DamageInfo.ImpactDistance = 0.0f;
}

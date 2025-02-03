#include "UArmor.h"

#include "UShellPool.h"
#include "AShell.h"
#include "FastLogger.h"
#include "Kismet/GameplayStatics.h"

UArmor::UArmor()
{
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<USoundWave> FireSoundAsset
	(TEXT("/Game/BuildingDestruction/Tank/MP_Sniper_Rifle.MP_Sniper_Rifle"));
	if (FireSoundAsset.Succeeded())
	{
		FireSound = FireSoundAsset.Object;
	}
}

void UArmor::BeginPlay()
{
	Super::BeginPlay();

	ShellPool = NewObject<UShellPool>(this, UShellPool::StaticClass());
}

void UArmor::FireShell(EShellID ShellID, FVector Location, FRotator Rotation, FName ProfileName, UClass* TargetClass)
{
	// 여기서 HitTrace를 통해 타겟이 너무 가까이 있으면 HitTrace로 발사하는 로직을 사용하자
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this->GetOwner());
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Location, Location + Rotation.Vector() * 10000, ECC_Visibility, Params))
	{
		if (HitResult.Distance < 1000)
		{
			HitscanShot(ShellID, HitResult, TargetClass);
			return ;
		}
	}
	ProjectileShot(ShellID, Location, Rotation, ProfileName, TargetClass);

	// Sound Play For Fire
	UGameplayStatics::PlaySoundAtLocation(this, FireSound, Location, 1.0f, 1.0f, 0.0f, nullptr);
}

void UArmor::ReleaseShell(EShellID ShellID, AShell* Shell)
{
	ShellPool->ReleaseShell(ShellID, Shell);
}

void UArmor::ProjectileShot(EShellID ShellID, FVector Location, FRotator Rotation, FName ProfileName, UClass* TargetClass)
{
	AShell* Shell = ShellPool->AcquireShell(ShellID, this);
	if (!Shell)
	{
		return ;
	}
	Shell->SetTarget(TargetClass);
	Shell->SetActorLocation(Location + Rotation.Vector() * 650);
	Shell->SetActorRotation(Rotation);
	Shell->SetCollisionPreset(ProfileName);
	Shell->Active();
}

void UArmor::HitscanShot(EShellID ShellID, const FHitResult& HitResult, UClass* TargetClass)
{
	AShell* Shell = ShellPool->AcquireShell(ShellID, this);
	if (!Shell)
	{
		return ;
	}
	Shell->SetTarget(TargetClass);
	Shell->HitScanShot(HitResult);
}

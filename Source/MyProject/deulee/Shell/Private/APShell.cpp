#include "APShell.h"

#include "FastLogger.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "ShellDecalComponent.h"
#include "UArmor.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
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

	for (int32 i = 0; i < ExplosionPoolSize; i++)
	{
		UNiagaraComponent* ExplosionNS = NewObject<UNiagaraComponent>(this, UNiagaraComponent::StaticClass());
		ExplosionNS->SetupAttachment(Collision);
		ExplosionNS->SetAsset(ExplosionNSAsset);
		ExplosionNS->SetAutoActivate(false);
		ExplosionNS->RegisterComponent();
		ExplosionNS->Deactivate();
		ExplosionNSPool.Add(ExplosionNS);
	}
	for (int32 i = 0; i < ExplosionPoolSize; i++)
	{
		FTimerHandle TimerHandle;
		ExplosionTimerHandles.Add(TimerHandle);
	}
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
	ProcessShellAttack(OtherActor, SweepResult);
}

void APShell::WrapShellDamageInfo()
{
	DamageInfo.ImpactNormal = GetActorForwardVector() * -1;
	DamageInfo.ShellBasicInfo = ShellInfo;
	DamageInfo.bAbove = false;
	DamageInfo.ImpactDistance = 0.0f;
}

void APShell::SpawnExplodeEffect(const FHitResult& HitResult)
{
	FVector ImpactPoint = HitResult.ImpactPoint;
	FRotator ImpactNormalRotator = UKismetMathLibrary::MakeRotFromX(HitResult.ImpactNormal);

	// 풀에서 사용 가능한 이펙트를 가져옴
	int32 Index = 0;
	UNiagaraComponent* NiagaraComp = nullptr;
	for (UNiagaraComponent* Effect : ExplosionNSPool)
	{
		if (!Effect->IsActive())
		{
			NiagaraComp = Effect;
			break;
		}
		Index++;
	}

	if (NiagaraComp)
	{
		// 일시적으로 Detach함
		NiagaraComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		
		// 이펙트 위치 및 회전 설정
		NiagaraComp->SetUsingAbsoluteLocation(true);
		NiagaraComp->SetUsingAbsoluteRotation(true);
		NiagaraComp->SetUsingAbsoluteScale(true);
		
		NiagaraComp->SetWorldLocation(ImpactPoint);
		NiagaraComp->SetWorldRotation(ImpactNormalRotator);

		// 이펙트 활성화
		NiagaraComp->Activate(true);

		// 2초후 비활성화
		TWeakObjectPtr<UNiagaraComponent> WeakNiagaraComp = NiagaraComp;
		TWeakObjectPtr<APShell> WeakThis = this;
		GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandles[Index], [WeakNiagaraComp, WeakThis, Index]()
		{
			if (!(WeakNiagaraComp.IsValid() && WeakThis.IsValid()))
			{
				return ;
			}
			APShell* Shell = WeakThis.Get();
			Shell->ReleaseNiagaraEffect(WeakNiagaraComp.Get(), Shell, Index);
		}, 0.5f, false);
	}
}

void APShell::ReleaseNiagaraEffect(UNiagaraComponent* NiagaraComp, APShell* Shell, int32 Index)
{
	// 이펙트 비활성화
	NiagaraComp->Deactivate();

	// 다시 셸의 Collision 컴포넌트에 Attach
	NiagaraComp->AttachToComponent(
		this->Collision,
		FAttachmentTransformRules::KeepRelativeTransform
	);

	// 상대 좌표 모드로 복귀
	NiagaraComp->SetUsingAbsoluteLocation(false);
	NiagaraComp->SetUsingAbsoluteRotation(false);
	NiagaraComp->SetUsingAbsoluteScale(false);
}

void APShell::HitScanShot(const FHitResult& HitResult)
{
	Super::HitScanShot(HitResult);

	ProcessShellAttack(HitResult.GetActor(), HitResult);
}

void APShell::ProcessShellAttack(AActor* OtherActor, const FHitResult& SweepResult)
{
	// TODO: Add Shell ID Enum
	WrapShellDamageInfo();

	// 인터페이스가 없거나 ShellTargetClass가 아닌 경우에는 데칼을 생성
	if (!OtherActor->GetClass()->ImplementsInterface(UDamageInterface::StaticClass())
		|| !OtherActor->IsA(ShellTargetClass))
	{
		ShellDecalComponent->SpawnDecal(SweepResult, false);
	}
	else
	{
		IDamageInterface::Execute_TakeDamage(OtherActor, DamageInfo);
		ShellDecalComponent->SpawnDecal(SweepResult, true);
	}

	SpawnExplodeEffect(SweepResult);
	
	DeActive();
	Armor->ReleaseShell(ShellInfo.ShellID, this);
	// Debug Draw
	DrawDebugSphere(GetWorld(), SweepResult.ImpactPoint, 10.0f, 12, FColor::Red, false, 2.0f);
}
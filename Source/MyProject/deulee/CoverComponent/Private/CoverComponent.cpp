#include "CoverComponent.h"

#include "CoverActorBase.h"
#include "FastLogger.h"
#include "Kismet/KismetMaterialLibrary.h"

UCoverComponent::UCoverComponent()
{
}

void UCoverComponent::BeginPlay()
{
	Super::BeginPlay();

	OuterCache = GetOwner();

	MeshComponents.Empty();
	OuterCache->GetComponents<UStaticMeshComponent>(MeshComponents);

	SkeletalMeshComponents.Empty();
	OuterCache->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);
	GetWorld()->GetTimerManager().SetTimer(CoverTimerHandle, this, &UCoverComponent::SetCoverValue, 0.5f, true);
}

void UCoverComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(CoverTimerHandle);
}

void UCoverComponent::SetCoverValue()
{
	CoverRender = true;
	// Line Trace사용하여 커버를 찾는다.
	FVector Start = GetComponentLocation();
	APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!PlayerPawn)
	{
		return;
	}
	FVector Player = PlayerPawn->GetActorLocation();

	// Line Trace에 CoverActorBase를 제외하고는 다른 액터들은 무시한다.
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());
	CollisionParams.AddIgnoredActor(GetWorld()->GetFirstPlayerController()->GetPawn());

	TArray<FHitResult> HitResults;
	GetWorld()->LineTraceMultiByChannel(HitResults, Start, Player, ECC_GameTraceChannel7, CollisionParams); // ECC_GameTraceChannel7은 CoverActorBase
	// 어차피 CoverActorBase만 찾아냄
	for (auto& HitResult : HitResults)
	{
		FFastLogger::LogScreen(FColor::Red, TEXT("HitResult : %s"), *HitResult.GetActor()->GetName());
		ACoverActorBase* CoverActor = Cast<ACoverActorBase>(HitResult.GetActor());
		if (CoverActor && CoverActor->IsCoverable())
		{
			CoverRender = false;
			break;
		}
	}

	for (auto& MeshComponent : MeshComponents)
	{
		// MeshComponent->bRenderCustomDepth = CoverRender;
		MeshComponent->SetRenderCustomDepth(CoverRender);
	}
}

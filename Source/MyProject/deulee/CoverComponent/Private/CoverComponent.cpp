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
	for (UStaticMeshComponent* MeshComponent : MeshComponents)
	{
		if (UMaterialInterface* OutlineMaterial = MeshComponent->GetOverlayMaterial())
		{
			UMaterialInstanceDynamic* OutlineMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), OutlineMaterial);
			MeshComponent->SetOverlayMaterial(OutlineMaterialInstance);
			OutlineMaterialInstances.Add(OutlineMaterialInstance);
		}
	}

	SkeletalMeshComponents.Empty();
	OuterCache->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);
	for (USkeletalMeshComponent* SkeletalMeshComponent : SkeletalMeshComponents)
	{
		if (UMaterialInterface* OutlineMaterial = SkeletalMeshComponent->GetOverlayMaterial())
		{
			UMaterialInstanceDynamic* OutlineMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), OutlineMaterial);
			SkeletalMeshComponent->SetOverlayMaterial(OutlineMaterialInstance);
			OutlineMaterialInstances.Add(OutlineMaterialInstance);
		}
	}
	
	GetWorld()->GetTimerManager().SetTimer(CoverTimerHandle, this, &UCoverComponent::SetCoverValue, 0.5f, true);
}

void UCoverComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(CoverTimerHandle);
}

void UCoverComponent::SetCoverValue()
{
	CoverValue = 1.0f;
	// Line Trace사용하여 커버를 찾는다.
	FVector Start = GetComponentLocation();
	FVector Player = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();

	// Line Trace에 CoverActorBase를 제외하고는 다른 액터들은 무시한다.
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());
	CollisionParams.AddIgnoredActor(GetWorld()->GetFirstPlayerController()->GetPawn());

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, Player, ECC_GameTraceChannel7, CollisionParams);
	if (bHit)
	{
		ACoverActorBase* CoverActor = Cast<ACoverActorBase>(HitResult.GetActor());
		if (CoverActor && CoverActor->IsCoverable())
		{
			CoverValue = 0.0f;
		}
	}
	// Hit된 액터가 CoverActorBase를 상속받았는지 확인한다.
	for (UMaterialInstanceDynamic* OutlineMaterialInstance : OutlineMaterialInstances)
	{
		OutlineMaterialInstance->SetScalarParameterValue("LineThickness", CoverValue * 5.0f);
	}
}

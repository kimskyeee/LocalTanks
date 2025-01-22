#include "ShellDecalComponent.h"

#include "FastLogger.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetMathLibrary.h"

UShellDecalComponent::UShellDecalComponent()
{
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_BulletDecal
	(TEXT("/Game/Material/BulletDecal/Instances/M_ImpactDecal_Inst.M_ImpactDecal_Inst"));
	if (MI_BulletDecal.Succeeded())
	{
		ShellDecalMaterialInstance = MI_BulletDecal.Object;
	}
}

void UShellDecalComponent::BeginPlay()
{
	Super::BeginPlay();

	ShellDecalMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), ShellDecalMaterialInstance);
}

void UShellDecalComponent::SpawnDecal(const FHitResult& HitResult, bool bHitTank)
{
	FVector ImpactPoint = HitResult.ImpactPoint;
	FVector ImpactNormal = HitResult.ImpactNormal;
	
	UPrimitiveComponent* HitComponent = HitResult.GetComponent();
	FVector ReverseImpactNormal = ImpactNormal * -1;

	FRotator RotatorFromRIN = UKismetMathLibrary::Conv_VectorToRotator(ReverseImpactNormal);
	float RandomRoll = UKismetMathLibrary::RandomFloatInRange(-180.0f, 180.0f);

	FRotator DecalRotation = FRotator(RotatorFromRIN.Pitch, RotatorFromRIN.Yaw, RandomRoll);
	
	int Frame = UKismetMathLibrary::RandomIntegerInRange(0, 3);

	FVector DecalSize = bHitTank ? FVector(50.0f, 50.0f, 50.0f) : FVector(200.0f, 200.0f, 200.0f);
	ShellDecalMaterial->SetScalarParameterValue(TEXT("Frame"), Frame);

	UDecalComponent* Decal = UGameplayStatics::SpawnDecalAttached(ShellDecalMaterial, DecalSize, HitComponent, NAME_None, ImpactPoint, DecalRotation, EAttachLocation::KeepWorldPosition, 20.0f);
	
	Decal->SetFadeScreenSize(0.0f);
	Decal->SetFadeOut(10.0f, 10.0f, false);
}

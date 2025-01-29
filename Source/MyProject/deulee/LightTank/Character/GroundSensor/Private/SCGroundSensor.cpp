#include "SCGroundSensor.h"

#include "FastLogger.h"
#include "OccupiedZone.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

USCGroundSensor::USCGroundSensor()
{
	PrimaryComponentTick.bCanEverTick = true;
	Super::SetComponentTickEnabled(true);
}

void USCGroundSensor::BeginPlay()
{
	Super::BeginPlay();
}

// LineTrace를 진행하여 땅에 닿았는지 여부를 판단하고 필요한 벡터들을 추출함. (땅의 위치, 땅의 법선 벡터)
void USCGroundSensor::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    FVector Start = GetComponentLocation();
    FVector End = Start - (GetUpVector() * TraceDistance); 
    
    FHitResult HitResult;
    
    FCollisionQueryParams TraceParams;
    TraceParams.AddIgnoredActor(GetOwner());
    AOccupiedZone* OccupiedZone = Cast<AOccupiedZone>(UGameplayStatics::GetActorOfClass(GetWorld(), AOccupiedZone::StaticClass()));
    if (OccupiedZone)
    {
        TraceParams.AddIgnoredActor(OccupiedZone);
    }
    
    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, TraceParams))
    {
        bOnGround = HitResult.bBlockingHit;
        HitLocation = HitResult.Location;
        HitNormal = HitResult.Normal;
    }
    else
    {
        bOnGround = false;
    }
}

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
    FFastLogger::LogFile(TEXT("LocalTankErrorRep.txt"), TEXT("Start : %s"), *Start.ToString());
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

        // 성공한 경우 녹색 디버그 라인 그리기
        DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.1f, 0, 2.0f);
    }
    else
    {
        bOnGround = false;
        // 실패한 경우 빨간색 디버그 라인 그리기
        DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.1f, 0, 2.0f);
    }
    if (bOnGround)
    {
    }
}

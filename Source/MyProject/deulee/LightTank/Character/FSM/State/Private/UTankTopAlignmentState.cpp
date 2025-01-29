#include "UTankTopAlignmentState.h"
#include "ACLightTankFSM.h"
#include "ALightTankCharacter.h"
#include "FastLogger.h"
#include "GeometryTypes.h"
#include "ShellBasicInfo.h"
#include "Components/ArrowComponent.h"
#include "ShellLibrary.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

UTankTopAlignmentState::UTankTopAlignmentState()
{
}

void UTankTopAlignmentState::Enter(ALightTankCharacter* Tank, UACLightTankFSM* FSM)
{
	Super::Enter(Tank, FSM);
}

void UTankTopAlignmentState::Update(ALightTankCharacter* Tank, UACLightTankFSM* FSM, float DeltaTime)
{
	UTankState::Update(Tank, FSM, DeltaTime);

	bCanAttackTarget = true;
	// 탱크의 포신이 상대방과 정렬되어야 함
	SetTurretTargetAngle(Tank);
	SetGunTargetAngle(Tank);
}

void UTankTopAlignmentState::Exit(ALightTankCharacter* Tank, UACLightTankFSM* FSM)
{
	Super::Exit(Tank, FSM);
}

bool UTankTopAlignmentState::CanAttackTarget() const
{
	return bCanAttackTarget;
}

void UTankTopAlignmentState::CheckAttackCondition(float TargetTheta, float CurrentTheta, bool Debug)
{
	float NormalizedTarget = UKismetMathLibrary::NormalizeAxis(TargetTheta);
	float NormalizedCurrent = UKismetMathLibrary::NormalizeAxis(CurrentTheta);

	float TargetDelta = NormalizedTarget - NormalizedCurrent;
	TargetDelta = UKismetMathLibrary::NormalizeAxis(TargetDelta);
	
	if (FMath::Abs(TargetDelta) < 0.2f)
	{
		bCanAttackTarget &= true;
		
	}
	else
	{
		bCanAttackTarget &= false;
	}
}

void UTankTopAlignmentState::SetTurretTargetAngle(ALightTankCharacter* Tank)
{
	// 1. 탱크의 현재 월드 위치와 회전 가져오기
	FVector TurretWorldLocation = Tank->GetActorLocation();
	FRotator TankWorldRotation = Tank->GetActorRotation();

	AActor* Target = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!Target)
	{
		return ;
	}
	FVector TargetWorldLocation = Target->GetActorLocation();

	// 2. 목표물의 위치를 포신의 로컬 좌표계로 변환
	FVector RelativeTargetLocation = Tank->GetTransform().InverseTransformPosition(TargetWorldLocation);

	// 3. 목표물의 Yaw 각도 계산 (로컬 좌표계 기준)
	double TargetYawRad = FMath::Atan2(RelativeTargetLocation.Y, RelativeTargetLocation.X);
	double TargetYawDeg = FMath::RadiansToDegrees(TargetYawRad);

	Tank->SetTurretTargetAngle(TargetYawDeg);

	// 4. 발사 가능 상태인지 업데이트
	CheckAttackCondition(TargetYawDeg, Tank->GetTurretCollision()->GetRelativeRotation().Yaw);
}

void UTankTopAlignmentState::SetGunTargetAngle(ALightTankCharacter* Tank)
{
	FShellBasicInfo ShellInfo = UShellLibrary::GetShellBasicInfo(Tank->GetShellID());
	
	constexpr float Gravity = 980.f;

	// 0. 기준이 되는 로컬 좌표계를 가져옴
	FTransform BaseTransform = Tank->GetTurretCollision()->GetComponentTransform();

	// 1. 포신의 현재 월드 위치와 회전 가져오기
	FTransform GunTransform = Tank->GetGunCollision()->GetComponentTransform();

	// 2. 목표물의 위치를 포신의 로컬 좌표계로 변환
	AActor* Target = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!Target)
	{
		return ;
	}
	FVector TargetWorldLocation = Target->GetActorLocation();
	TargetWorldLocation += Tank->GetActorUpVector() * 50.f; // 플레이어의 머리 위로 조정
	FVector RelativeTargetLocation = BaseTransform.InverseTransformPosition(TargetWorldLocation);
	FVector RelativeGunLocation = BaseTransform.InverseTransformPosition(GunTransform.GetLocation());
		
	// 3. 로컬 좌표계에서의 수평 거리 R과 높이 차 Delta h 계산
	double R = FMath::Sqrt(FMath::Square(RelativeTargetLocation.X - RelativeGunLocation.X) + FMath::Square(RelativeTargetLocation.Y - RelativeGunLocation.Y));
	double DeltaH = RelativeTargetLocation.Z - RelativeGunLocation.Z; // Unreal에서 Z축이 Up 방향

	// 4. 탄도 방정식의 판별식 계산
	double V = ShellInfo.ShellSpeed;
	double V2 = V * V;
	double V4 = V2 * V2;
	double Discriminant = V4 - Gravity * (Gravity * R * R + 2.0 * DeltaH * V2);

	// 5. 판별식이 음수이면 해가 없음
	if (Discriminant < 0.0)
	{
		return;
	}

	// 6. 두 개의 가능한 Pitch 각도 계산 (낮은 각도, 높은 각도)
	double SqrtDisc = FMath::Sqrt(Discriminant);

	// 낮은 각도 해
	double TanTheta1 = (V2 - SqrtDisc) / (Gravity * R);
	double ThetaRad1 = FMath::Atan(TanTheta1);
	double ThetaDeg1 = FMath::RadiansToDegrees(ThetaRad1);

	// 높은 각도 해
	double TanTheta2 = (V2 + SqrtDisc) / (Gravity * R);
	double ThetaRad2 = FMath::Atan(TanTheta2);
	double ThetaDeg2 = FMath::RadiansToDegrees(ThetaRad2);

	// 7. Pitch 각도를 낮은 각도부터 높은 각도 순으로 정렬
	TArray<double> PitchAngles;
	PitchAngles.Add(ThetaDeg1);
	PitchAngles.Add(ThetaDeg2);
	PitchAngles.Sort();

	// 8. 낮은 각도 선택
	double SelectedPitch = PitchAngles[0]; // 또는 상황에 맞게 선택

	// 9. 포신에 새로운 회전 적용
	Tank->SetGunTargetAngle(SelectedPitch);

	// 발사 가능한지 업데이트
	CheckAttackCondition(SelectedPitch, Tank->GetGunCollision()->GetRelativeRotation().Pitch, true);
}
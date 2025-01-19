#include "UMoveToCenterState.h"

#include "ACLightTankFSM.h"
#include "ALightTankCharacter.h"
#include "FastLogger.h"
#include "OccupiedZone.h"
#include "UAStarPathFinding.h"
#include "Kismet/GameplayStatics.h"

UMoveToCenterState::UMoveToCenterState()
{
	StateString = "MoveToCenter";
}

void UMoveToCenterState::Enter(ALightTankCharacter* Tank, UACLightTankFSM* FSM)
{
	Super::Enter(Tank, FSM);

	FFastLogger::LogScreen(FColor::Green, TEXT("Enter MoveToCenter State!"));
	// 목표 위치 설정
	// A* 알고리즘 들어갈곳
	FVector Start = Tank->GetActorLocation();
	SetCenter();

	// 초기화
	TArray<FVector> OutPath;
	PathFinding->ResetField();
	Tank->ClearGoals();
	TQueue<FVector2D>& GoalQueue = Tank->GetGoalQueue();
	
	PathFinding->FindPath(Start, Center, OutPath);
	if (OutPath.Num() == 0)
	{
		FFastLogger::LogScreen(FColor::Red, TEXT("No Path Found!"));
		return ;
	}
	else
	{
		// OutPath[0] = Start;
		OutPath[OutPath.Num() - 1] = Center;
	}

	for (FVector& Path : OutPath)
	{
		GoalQueue.Enqueue({Path.X, Path.Y});
	}
}

/*
	MoveToCenter --> ApproachEnemy: 적이 탐지 범위 진입
	MoveToCenter --> Damaged: 피격당함
	MoveToCenter --> Destroyed: 체력 0
	MoveToCenter --> Idle: 중앙에 도달
*/
void UMoveToCenterState::Update(ALightTankCharacter* Tank, UACLightTankFSM* FSM, float DeltaTime)
{
	Super::Update(Tank, FSM, DeltaTime);

	if (Tank->IsDestroyed())
	{
		FSM->ChangeState(ETankStateID::Destroyed);
		return;
	}

	if (Tank->IsDamaged())
	{
		FSM->ChangeState(ETankStateID::Damaged);
		return;
	}

	// 새로운 Path를 설정해야 하는 경우
	if (Tank->IsEnemyInDetectionRange() && IsPossibleToApproach(Tank))
	{
		Tank->ClearGoals();
		FSM->ChangeState(ETankStateID::ApproachEnemy);
		return;
	}

	if (Tank->IsInCenter())
    {
        FSM->ChangeState(ETankStateID::Idle);
        return;
    }

	// 실질적인 이동 : MoveToCenter의 경우 그냥 목표만 초반에 설정해주면 될듯.
}

void UMoveToCenterState::Exit(ALightTankCharacter* Tank, UACLightTankFSM* FSM)
{
	Super::Exit(Tank, FSM);
}

bool UMoveToCenterState::IsPossibleToApproach(ALightTankCharacter* Tank)
{
	// 적에게 일정 거리 안에 들어가기 위한 목표 설정
	FVector Start = Tank->GetActorLocation();
	AActor* PlayerActor = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!PlayerActor)
	{
		check(false)
	}
	FVector Player = PlayerActor->GetActorLocation();

	// 플레이어의 지점으로부터 일정 거리 떨어진 지점을 목표로 설정
	FVector Dir = Start - Player;
	Dir.Normalize();

	FVector Goal = Player + Dir * ApproachDistance;
	if (PathFinding->IsWall(Goal))
	{
		return false;
	}
	return true;
}

void UMoveToCenterState::SetCenter()
{
	AOccupiedZone* OccupiedZone = Cast<AOccupiedZone>(UGameplayStatics::GetActorOfClass(GetWorld(), AOccupiedZone::StaticClass()));
	if (OccupiedZone)
	{
		Center = OccupiedZone->GetActorLocation();
	}
}
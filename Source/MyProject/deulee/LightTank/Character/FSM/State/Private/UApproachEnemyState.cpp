#include "UApproachEnemyState.h"

#include "ACLightTankFSM.h"
#include "ALightTankCharacter.h"
#include "FastLogger.h"
#include "UAStarPathFinding.h"
#include "Kismet/GameplayStatics.h"

UApproachEnemyState::UApproachEnemyState()
{
	StateString = "ApproachEnemy";
}

void UApproachEnemyState::Enter(ALightTankCharacter* Tank, UACLightTankFSM* FSM)
{
	Super::Enter(Tank, FSM);

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
		Goal = Player;
    }

	PathFinding->ResetField();

	TArray<FVector> OutPath;
	PathFinding->FindPath(Start, Goal, OutPath);
	if (OutPath.Num() == 0)
    {
		FFastLogger::LogScreen(FColor::Red, TEXT("No Path Found!"));
		return ;
    }
    else
    {
        OutPath[OutPath.Num() - 1] = Goal;
    }

	TQueue<FVector2D>& GoalQueue = Tank->GetGoalQueue();
	for (FVector& Path : OutPath)
    {
        GoalQueue.Enqueue({Path.X, Path.Y});
    }
}

/*
	ApproachEnemy --> Destroyed: 체력 0
	ApproachEnemy --> Damaged: 피격당함
	ApproachEnemy --> AttackEnemy: 공격 사거리 안에 들어옴
	ApproachEnemy --> MoveToCenter: 탐지 사거리 밖으로 나감
	ApproachEnemy --> Retreating: 너무 가까움
	ApproachEnemy --> ApproachEnemy: Approach 목표 지점에 도착함 (근데 계속 추적해야 하는 상황)
*/
void UApproachEnemyState::Update(ALightTankCharacter* Tank, UACLightTankFSM* FSM, float DeltaTime)
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

	// 공격 사거리 안에 들었을 경우
	if (Tank->IsEnemyInAttackRange() && Tank->IsReloadedShell() && CanAttackTarget())
	{
        Tank->ClearGoals();
		FSM->ChangeState(ETankStateID::AttackEnemy);
		return;
	}

	// 탐지/공격 사거리 밖으로 나갔을 경우
	if (!Tank->IsEnemyInDetectionRange())
	{
		Tank->ClearGoals();
		FSM->ChangeState(ETankStateID::MoveToCenter);
		return;
	}

	// 너무 가까운 경우
	if (Tank->IsEnemyTooClose())
	{
		Tank->ClearGoals();
		FSM->ChangeState(ETankStateID::Retreating);
		return;
	}
	
	// 적에게 따라가는 로직 && 터렛 정렬 // Tank가 Final에 있다면 또 추적
	if (Tank->IsInFinalGoal())
    {
		Tank->ClearGoals();
        FSM->ChangeState(ETankStateID::ApproachEnemy);
        return;
    }

}

void UApproachEnemyState::Exit(ALightTankCharacter* Tank, UACLightTankFSM* FSM)
{
	Super::Exit(Tank, FSM);
}

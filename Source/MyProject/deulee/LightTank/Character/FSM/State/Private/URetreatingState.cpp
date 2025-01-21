#include "URetreatingState.h"

#include "ACLightTankFSM.h"
#include "ALightTankCharacter.h"
#include "FastLogger.h"
#include "UAStarPathFinding.h"
#include "Kismet/GameplayStatics.h"

URetreatingState::URetreatingState()
{
	StateString = "Retreating";

	// PathFinding = CreateDefaultSubobject<UAStarPathFinding>(TEXT("PathFindingRetreating"));
}

// README: Game Play Tag - Multi State 관리 키워드
void URetreatingState::Enter(ALightTankCharacter* Tank, UACLightTankFSM* FSM)
{
	Super::Enter(Tank, FSM);

	// 적에게 일정 거리 안에 들어가기 위한 목표 설정
	FVector Start = Tank->GetActorLocation();
	AActor* PlayerActor = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!PlayerActor)
    {
        FFastLogger::LogScreen(FColor::Red, TEXT("Player Not Found!"));
        check(false)
    }
	FVector Player = PlayerActor->GetActorLocation();

	FVector Start2D = FVector(Start.X, Start.Y, 0);
	FVector End2D = FVector( Player.X,  Player.Y, 0);

	// 플레이어의 지점으로부터 일정 거리 떨어진 지점을 목표로 설정
	FVector Dir = Start - Player;
	Dir.Normalize();
	
	FVector Goal = Player + Dir * RetreatDistance;
	while (true)
	{
		if (PathFinding->IsWall(Goal))
		{
			float TileWidth = 2000;
			Goal = Player + Dir * TileWidth;
		}
		else
		{
			break;
		}
	}
	
	// 기존의 골이 있으면 다시 세팅 ㄴㄴ
	TQueue<FVector2D>& GoalQueue = Tank->GetGoalQueue();
	// @fixme : 여기 빵꾸 있음

	// TODO : 도망 다 치면 Attack State인데 다시 접근하게 되면 bSkipSetting 때문에 못 도망침..
	if (!Tank->IsInFinalGoal() && bSkipSetting)
	{
		return ;
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
	
	for (FVector& Path : OutPath)
	{
		GoalQueue.Enqueue({Path.X, Path.Y});
	}
}

/*
	Retreating --> Destroyed: 체력 0
	Retreating --> Damaged: 피격당함
	Retreating --> MoveToCenter: 적이 탐지 범위 밖으로 나감
	Retreating --> AttackEnemy: 공격 사거리 안에 들어옴 및 공격 가능
	Retreating --> Retreating: 계속해서 도망가야 하는 상황
 */
void URetreatingState::Update(ALightTankCharacter* Tank, UACLightTankFSM* FSM, float DeltaTime)
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

	if (Tank->IsEnemyOutOfRange())
	{
		Tank->ClearGoals();
		FSM->ChangeState(ETankStateID::MoveToCenter);
		return;
	}
	
	// 적이 공격 범위 안에 있을 경우 계속해서 공격
	// 사실 도망중이라는 것은 계속해서 범위 안에 있다는 뜻이기도 함.
	
	/*
	 * 도망가는 로직 진행
	 */
	
	// 도망가는 로직이 끝나면 현재 여기서 무한 재귀 도는 중 만약 이전에 도망치던 상태였다면 초기화를 다시 해주면
	// 무한 굴레에 빠짐.. 
	if (Tank->IsEnemyInAttackRange() && Tank->IsReloadedShell() && CanAttackTarget())
	{
		FSM->ChangeState(ETankStateID::AttackEnemy);
		return;
	}
	else
	{
		if (FSM->GetPrevState()->GetID() != ETankStateID::Retreating)
		{
			Tank->ClearGoals();
		}
		// 아래의 로직을 주석 해제하면 계속해서 도망가는 위치를 구하게 된다. 즉, 업데이트가 빠름
		// else
		// {
		// 	Cast<URetreatingState>(FSM->GetPrevState())->bSkipSetting = true;	
		// }
		FSM->ChangeState(ETankStateID::Retreating);
		return;
	}
}

void URetreatingState::Exit(ALightTankCharacter* Tank, UACLightTankFSM* FSM)
{
	bSkipSetting = false;
	Super::Exit(Tank, FSM);
}

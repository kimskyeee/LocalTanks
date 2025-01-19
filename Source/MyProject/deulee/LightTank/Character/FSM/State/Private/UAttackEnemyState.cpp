#include "UAttackEnemyState.h"

#include "ACLightTankFSM.h"
#include "ALightTankCharacter.h"
#include "FastLogger.h"
#include "UAStarPathFinding.h"
#include "URetreatingState.h"

void UAttackEnemyState::Enter(ALightTankCharacter* Tank, UACLightTankFSM* FSM)
{
	Super::Enter(Tank, FSM);

	// 공격 로직 (포탄 발사, 타이머 초기화)
	// 앞에서 Bool 체크를 해서 공격이 가능한 상태인지 확인 못하면 공격을 못하게 막아야함
	FFastLogger::LogScreen(FColor::Red, TEXT("Enter Attack State!"));

	Tank->Fire();
}

/*
	AttackEnemy --> Retreating: 적이 너무 가까워짐
	AttackEnemy --> ApproachEnemy: 공격 사거리 벗어남 및 접근 가능
	AttackEnemy --> MoveToCenter: 공격 사거리 벗어남 및 접근 불가능
	AttackEnemy --> Damaged: 피격당함
	AttackEnemy --> Destroyed: 체력 0
 */
void UAttackEnemyState::Update(ALightTankCharacter* Tank, UACLightTankFSM* FSM, float DeltaTime)
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

	if (Tank->IsEnemyTooClose())
	{
		if (FSM->GetPrevState()->GetID() != ETankStateID::Retreating)
		{
			Tank->ClearGoals();
		}
		else
		{
			Cast<URetreatingState>(FSM->GetPrevState())->bSkipSetting = true;	
		}
		FSM->ChangeState(ETankStateID::Retreating);
		return;
	}

	// 탐지/공격 사거리 밖으로 나갔을 경우
	if (Tank->IsEnemyOutOfRange())
	{
		if (IsPossibleToApproach(Tank))
		{
			Tank->ClearGoals();
			FSM->ChangeState(ETankStateID::ApproachEnemy);
		}
		else
		{
			Tank->ClearGoals();
			FSM->ChangeState(ETankStateID::MoveToCenter);
		}
		return;
	}

	if (Tank->IsReloadedShell() && CanAttackTarget())
	{
		FSM->ChangeState(ETankStateID::AttackEnemy);
		return;
	}
}

void UAttackEnemyState::Exit(ALightTankCharacter* Tank, UACLightTankFSM* FSM)
{
	Super::Exit(Tank, FSM);

	// 공격을 끝내는 모션을 취해야 함
}

bool UAttackEnemyState::IsPossibleToApproach(ALightTankCharacter* Tank)
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

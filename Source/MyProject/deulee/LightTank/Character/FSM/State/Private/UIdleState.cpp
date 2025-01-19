#include "UIdleState.h"

#include "ACLightTankFSM.h"
#include "ALightTankCharacter.h"
#include "FastLogger.h"

void UIdleState::Enter(ALightTankCharacter* Tank, UACLightTankFSM* FSM)
{
	Super::Enter(Tank, FSM);
	
	// 중앙(목표 지점)에 도착한 상태
	// 이동을 멈추고 대기 상태로 전환
	// 이동 명령을 멈추는 로직

	FFastLogger::LogScreen(FColor::Green, TEXT("Enter Idle State!"));
}

/*
	Idle --> Destroyed: 체력 0
	Idle --> Damaged: 피격당함
	Idle --> ApproachEnemy: 적이 탐지 범위 진입
*/
void UIdleState::Update(ALightTankCharacter* Tank, UACLightTankFSM* FSM, float DeltaTime)
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

	// 적이 탐지/공격 범위에 들어왔을 경우
	if (Tank->IsEnemyInDetectionRange())
	{
		FSM->ChangeState(ETankStateID::ApproachEnemy);
		return;
	}

	// 속도 감쇠 로직 (속도가 0이 될 때까지)
}

void UIdleState::Exit(ALightTankCharacter* Tank, UACLightTankFSM* FSM)
{
	Super::Exit(Tank, FSM);
}

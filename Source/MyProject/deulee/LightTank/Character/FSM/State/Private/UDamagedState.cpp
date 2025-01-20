#include "UDamagedState.h"

#include "ACLightTankFSM.h"
#include "ALightTankCharacter.h"
#include "OccupationComponent.h"

void UDamagedState::Enter(ALightTankCharacter* Tank, UACLightTankFSM* FSM)
{
	Super::Enter(Tank, FSM);

	Tank->SetDamaged(false);
	Tank->GetOccupationComponent()->StartOccupationDelayed();
}

void UDamagedState::Update(ALightTankCharacter* Tank, UACLightTankFSM* FSM, float DeltaTime)
{
	Super::Update(Tank, FSM, DeltaTime);

	if (Tank->IsDestroyed())
	{
		FSM->ChangeState(ETankStateID::Destroyed);
		return;
	}

	// 스턴 중에 또 공격 받는 경우
	if (Tank->IsDamaged())
	{
		FSM->ChangeState(ETankStateID::Damaged);
		return;
	}
	
	// 적에 따라 다음 상태로 전이
	if (Tank->IsEnemyInDetectionRange())
	{
		FSM->ChangeState(ETankStateID::ApproachEnemy);
	}
	else
	{
		FSM->ChangeState(ETankStateID::MoveToCenter);
	}
}

void UDamagedState::Exit(ALightTankCharacter* Tank, UACLightTankFSM* FSM)
{
	Super::Exit(Tank, FSM);

	// 스턴 상태 초기화 하면 안됨
}

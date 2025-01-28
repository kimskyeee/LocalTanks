#include "SnippingState.h"

#include "ACLightTankFSM.h"
#include "ALightTankCharacter.h"

void USnippingState::Enter(ALightTankCharacter* Tank, UACLightTankFSM* FSM)
{
	Super::Enter(Tank, FSM);
}

void USnippingState::Update(ALightTankCharacter* Tank, UACLightTankFSM* FSM, float DeltaTime)
{
	Super::Update(Tank, FSM, DeltaTime);

	if (Tank->IsDestroyed())
	{
		FSM->ChangeState(ETankStateID::Destroyed);
		return;
	}

	// 가만히 계속 멀리서 공격
	if (CanAttackTarget() && Tank->IsReloadedShell())
	{
		Tank->Fire();
	}
}

void USnippingState::Exit(ALightTankCharacter* Tank, UACLightTankFSM* FSM)
{
	Super::Exit(Tank, FSM);
}

#include "ACLightTankFSM.h"

#include "ALightTankCharacter.h"
#include "FastLogger.h"
#include "SnippingState.h"
#include "UApproachEnemyState.h"
#include "UAttackEnemyState.h"
#include "UDamagedState.h"
#include "UDestroyedState.h"
#include "UIdleState.h"
#include "UMoveToCenterState.h"
#include "URetreatingState.h"
#include "TankState.h"


UACLightTankFSM::UACLightTankFSM()
{
	PrimaryComponentTick.bCanEverTick = true;
	Super::SetComponentTickEnabled(true);
}

void UACLightTankFSM::BeginPlay()
{
	Super::BeginPlay();

	OwnerActor = Cast<ALightTankCharacter>(GetOwner()) ? Cast<ALightTankCharacter>(GetOwner()) : nullptr;

	InitStatePool();
	
	if (OwnerActor->GetRoleID() == ETankRoleID::Sniper)
	{
		CurrentState = StatePool[ETankStateID::Snipping];
	}
	else
	{
		CurrentState = StatePool[ETankStateID::MoveToCenter];
	}
	
	CurrentState->Enter(OwnerActor, this);
}

void UACLightTankFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CurrentState->Update(OwnerActor, this, DeltaTime);
}

void UACLightTankFSM::ChangeState(ETankStateID NewStateID)
{
	CurrentState->Exit(OwnerActor, this);
	PrevState = CurrentState;
	CurrentState = StatePool[NewStateID];
	CurrentState->Enter(OwnerActor, this);
}

UTankState* UACLightTankFSM::GetCurrentState()
{
	return CurrentState;
}

UTankState* UACLightTankFSM::GetPrevState()
{
	return PrevState;
}

void UACLightTankFSM::InitStatePool()
{
	// 상태 객체 생성
	StatePool.Add(ETankStateID::Idle, NewObject<UIdleState>(this, UIdleState::StaticClass()));
	StatePool.Add(ETankStateID::MoveToCenter, NewObject<UMoveToCenterState>(this, UMoveToCenterState::StaticClass()));
	StatePool.Add(ETankStateID::ApproachEnemy, NewObject<UApproachEnemyState>(this, UApproachEnemyState::StaticClass()));
	StatePool.Add(ETankStateID::AttackEnemy, NewObject<UAttackEnemyState>(this, UAttackEnemyState::StaticClass()));
	StatePool.Add(ETankStateID::Retreating, NewObject<URetreatingState>(this, URetreatingState::StaticClass()));
	StatePool.Add(ETankStateID::Damaged, NewObject<UDamagedState>(this, UDamagedState::StaticClass()));
	StatePool.Add(ETankStateID::Destroyed, NewObject<UDestroyedState>(this, UDestroyedState::StaticClass()));
	StatePool.Add(ETankStateID::Snipping, NewObject<USnippingState>(this, USnippingState::StaticClass()));
	
	for (auto& State : StatePool)
	{
		State.Value->Initialize();
	}
}

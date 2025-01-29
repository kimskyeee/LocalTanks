#include "UDestroyedState.h"

#include "ALightTankCharacter.h"
#include "FastLogger.h"
#include "TankGameMode.h"

void UDestroyedState::Enter(ALightTankCharacter* Tank, UACLightTankFSM* FSM)
{
	Super::Enter(Tank, FSM);

	// 탱크 소멸 처리
	// 예: 리소스 정리, 장면에서 제거 등
	Tank->SetActorHiddenInGame(true);

	ATankGameMode* GameMode = Cast<ATankGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
    {
        GameMode->RespawnTank(Tank->GetRoleID());
    }
	Tank->Destroy();
}

void UDestroyedState::Update(ALightTankCharacter* Tank, UACLightTankFSM* FSM, float DeltaTime)
{
	Super::Update(Tank, FSM, DeltaTime);

	// 소멸 상태에서는 아무것도 하지 않음
}

void UDestroyedState::Exit(ALightTankCharacter* Tank, UACLightTankFSM* FSM)
{
	Super::Exit(Tank, FSM);

	// 일반적으로 소멸 상태에서는 Exit이 호출되지 않음. (왜냐면 이전에 소멸 처리 되기 때문)
}

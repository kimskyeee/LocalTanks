#include "TankGameMode.h"

#include "ALightTankCharacter.h"
#include "FastLogger.h"
#include "MyPawn.h"
#include "Blueprint/UserWidget.h"
#include "OccupiedZone.h"
#include "OutcomeUI.h"
#include "Kismet/GameplayStatics.h"
#include "MyProject/Mk/Character/Public/Mk_TankPawn.h"

ATankGameMode::ATankGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultPawnClass = nullptr;

	static ConstructorHelpers::FClassFinder<UOutcomeUI> WBP_PlayerWinnerWidgetClass
	(TEXT("/Game/NewSkye/GameUI/WBP_Victory.WBP_Victory_C"));
	if (WBP_PlayerWinnerWidgetClass.Succeeded())
	{
		PlayerWinnerWidgetClass = WBP_PlayerWinnerWidgetClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UOutcomeUI> WBP_AIWinnerWidgetClass
	(TEXT("/Game/NewSkye/GameUI/WBP_Defeat.WBP_Defeat_C"));
	if (WBP_AIWinnerWidgetClass.Succeeded())
	{
		AIWinnerWidgetClass = WBP_AIWinnerWidgetClass.Class;
	}
}

void ATankGameMode::RespawnTank(AActor* DestroyedActor)
{
	if (!DestroyedActor) return;

	UClass* DestroyedClass = DestroyedActor->GetClass();
	FFastLogger::LogScreen(FColor::Red, TEXT("%s Destroyed"), *DestroyedClass->GetName());
	APawn* SpawnActor = SpawnActorAtRandomPlace(DestroyedActor->GetClass());
	if (!SpawnActor) return;

	if (DestroyedClass == AI_LightTankClass)
	{
		AI_LightTanks.Remove(Cast<ALightTankCharacter>(DestroyedActor));
		AI_LightTanks.Add(Cast<ALightTankCharacter>(SpawnActor));
	}
	else if (DestroyedClass == SkyTankClass)
	{
		AI_SkyTanks.Remove(Cast<AMyPawn>(DestroyedActor));
		AI_SkyTanks.Add(Cast<AMyPawn>(SpawnActor));
	}
	else
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC)
		{
			PC->Possess(Cast<APawn>(SpawnActor));
		}
	}
}

APawn* ATankGameMode::SpawnActorAtRandomPlace(UClass* SpawnClass)
{
	float Radius = FMath::RandRange(15000.f, 18000.f);
	float Angle  = FMath::RandRange(0.f, 2.f * PI);

	float RandomX = Radius * FMath::Cos(Angle);
	float RandomY = Radius * FMath::Sin(Angle);

	// Z값 초기화
	float SpawnZ = 0.f;

	// 2. Line Trace로 땅의 높이 확인
	FVector StartPoint = FVector(RandomX, RandomY, 10000.f); // 위에서 시작
	FVector EndPoint = FVector(RandomX, RandomY, -10000.f); // 아래로 쏘기

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // 자신의 충돌 무시

	// 라인 트레이스 실행
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECC_Visibility, Params))
	{
		// 땅의 Z 좌표를 가져옴
		SpawnZ = HitResult.Location.Z;
	}

	float Offset = 200.f;
	if (SpawnClass == AI_LightTankClass || SpawnClass == SkyTankClass)
	{
		Offset = 700.f;
	}
	// 3. Offset 추가
	SpawnZ += Offset;

	// 4. 최종 위치 계산
	FVector SpawnLocation = FVector(RandomX, RandomY, SpawnZ);

	// 5. 액터 스폰
	FTransform T{};
	T.SetLocation(SpawnLocation);

	APawn* SpawnPawn = GetWorld()->SpawnActor<APawn>(SpawnClass, T);
	if (SpawnPawn)
	{
		SpawnPawn->AutoPossessPlayer = EAutoReceiveInput::Disabled;
	}
	return SpawnPawn;
};

void ATankGameMode::OnWinnerDetected(ETeam WinningTeam)
{
	switch (WinningTeam)
	{
	case ETeam::Player:
		PlayerWinnerWidget->AddToViewport();
		PlayerWinnerWidget->PlayAnimationFromCode();
		break;
	case ETeam::AI:
		AIWinnerWidget->AddToViewport();
		AIWinnerWidget->PlayAnimationFromCode();
		break;
	default:
		break;	
	}
}

void ATankGameMode::BeginPlay()
{
	Super::BeginPlay();

	AOccupiedZone* OccupiedZone = Cast<AOccupiedZone>(UGameplayStatics::GetActorOfClass(GetWorld(), AOccupiedZone::StaticClass()));
	if (OccupiedZone)
	{
		OccupiedZone->OnWinnerDetectedDelegate.AddDynamic(this, &ATankGameMode::OnWinnerDetected);
	}

	PlayerWinnerWidget = Cast<UOutcomeUI>(CreateWidget<UUserWidget>(GetWorld(), PlayerWinnerWidgetClass));
	AIWinnerWidget = Cast<UOutcomeUI>(CreateWidget<UUserWidget>(GetWorld(), AIWinnerWidgetClass));

	for (int i = 0; i < 2; i++)
	{
		AI_LightTanks.Add(Cast<ALightTankCharacter>(SpawnActorAtRandomPlace(AI_LightTankClass)));
		AI_SkyTanks.Add(Cast<AMyPawn>(SpawnActorAtRandomPlace(SkyTankClass)));
	}
}

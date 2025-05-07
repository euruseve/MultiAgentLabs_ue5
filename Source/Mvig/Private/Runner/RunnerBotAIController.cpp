// Fill out your copyright notice in the Description page of Project Settings.


#include "Runner/RunnerBotAIController.h"
#include "NavigationSystem.h"

void ARunnerBotAIController::BeginPlay()
{
	Super::BeginPlay();
	MoveToRandomPoint();
}

void ARunnerBotAIController::MoveToRandomPoint()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	FNavLocation RandomLocation;
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

	if (NavSys && NavSys->GetRandomPointInNavigableRadius(ControlledPawn->GetActorLocation(), 1000.f, RandomLocation))
	{
		MoveToLocation(RandomLocation.Location);
	}
}

void ARunnerBotAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	GetWorld()->GetTimerManager().SetTimer(WaitTimerHandle, this, &ARunnerBotAIController::MoveToRandomPoint, 2.0f, false);
}

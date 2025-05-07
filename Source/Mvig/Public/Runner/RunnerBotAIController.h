// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RunnerBotAIController.generated.h"

UCLASS()
class MVIG_API ARunnerBotAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

private:
	void MoveToRandomPoint();
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	FTimerHandle WaitTimerHandle;
};

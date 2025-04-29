// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TCPClient.h"
#include "GameFramework/Character.h"
#include "CriticBot.generated.h"

UCLASS()
class MVIG_API ACriticBot : public ACharacter
{
	GENERATED_BODY()

public:
	ACriticBot();
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	FTCPClient TcpClient;
	FVector2D GridPosition;
	FVector2D GoalPosition;
	FVector TargetWorldLocation;
	float GridSize = 200.f;

	FTimerHandle DecisionTimer;
	TArray<FVector> DebugPath;

	void MakeDecision();
	void MoveInDirection(int32 Action);
	void DrawPath();
	bool IsCloseToTarget() const;
	void SendTrainingData(float Reward, bool bDone);
};

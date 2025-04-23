// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TCPClient.h"
#include "GameFramework/Character.h"
#include "QBot.generated.h"

UCLASS()
class MVIG_API AQBot : public ACharacter
{
	GENERATED_BODY()

public:
	AQBot();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	FVector2D GridPosition;
	FVector2D GoalPosition;
	FVector TargetWorldLocation;
	float GridSize = 200.f;

	FTCPClient TcpClient;
	FTimerHandle DecisionHandle;
	
	void DrawFinalGoal(); 
	void DrawNextMove();  
	void DrawGrid();      
	void MakeDecision();
	
	void MoveAgent(int32 Action);
	void SendReward(float Reward, const FVector2D& NextState, bool bDone);
	void GenerateNewGoal();
	bool IsCloseToTarget() const;
};

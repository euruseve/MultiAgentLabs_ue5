// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TCPClient.h" 
#include "GameFramework/Character.h"
#include "NeuralBot.generated.h"

UCLASS()
class MVIG_API ANeuralBot : public ACharacter
{
	GENERATED_BODY()

public:
	ANeuralBot();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	FTCPClient TcpClient;
	FTimerHandle BrainTickHandle;

	FVector TargetLocation = FVector(500.0f, 500.0f, 0.0f); // Статична ціль

	FVector LastMoveDirection = FVector::ZeroVector;

	void SendStateAndReceiveDirection();
};

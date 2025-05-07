// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TCPClient.h"
#include "GameFramework/Character.h"
#include "TargetBot.generated.h"

UCLASS()
class MVIG_API ATargetBot : public ACharacter
{
	GENERATED_BODY()

public:
	ATargetBot();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ATargetPoint* TargetPoint;

private:
	void ConnectToServer();
	void RequestPath();
	void MoveAlongPath(float DeltaTime);

	FSocket* Socket;
	TArray<FVector> PathPoints;
	int32 CurrentTargetIndex;

	FString ServerIP = TEXT("127.0.0.1");
	int32 ServerPort = 9000;

	bool bIsConnected = false;
	bool bPathReceived = false;
};

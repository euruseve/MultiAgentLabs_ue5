// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RunnerBot.generated.h"

UCLASS()
class MVIG_API ARunnerBot : public ACharacter
{
	GENERATED_BODY()

public:
	ARunnerBot();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#include "Runner/RunnerBot.h"

ARunnerBot::ARunnerBot()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ARunnerBot::BeginPlay()
{
	Super::BeginPlay();
}

void ARunnerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


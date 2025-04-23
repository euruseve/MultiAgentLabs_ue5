// Fill out your copyright notice in the Description page of Project Settings.


#include "TCP/CriticBot.h"

// Sets default values
ACriticBot::ACriticBot()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACriticBot::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACriticBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACriticBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


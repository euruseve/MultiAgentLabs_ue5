// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Grid/GridManager.h"

#include "PathBenchmarkActor.generated.h"

UCLASS()
class MVIG_API APathBenchmarkActor : public AActor
{
	GENERATED_BODY()
	
public:	
	APathBenchmarkActor();

	UPROPERTY(EditAnywhere)
	AGridManager* GridManagerRef;

	UPROPERTY(EditAnywhere)
	FVector StartPos;

	UPROPERTY(EditAnywhere)
	FVector EndPos;
	
protected:
	virtual void BeginPlay() override;
	
};

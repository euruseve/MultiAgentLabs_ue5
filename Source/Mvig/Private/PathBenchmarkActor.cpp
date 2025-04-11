// Fill out your copyright notice in the Description page of Project Settings.

#include "PathBenchmarkActor.h"

#include "Pathfinders/AStarPathfinder.h"
#include "Pathfinders/DijkstraPathfinder.h"
#include "Pathfinders/BFSPathfinder.h"
#include "DrawDebugHelpers.h"

APathBenchmarkActor::APathBenchmarkActor()
{

}

void APathBenchmarkActor::BeginPlay()
{
	Super::BeginPlay();

	if (!GridManagerRef)
	{
		UE_LOG(LogTemp, Error, TEXT("GridManagerRef is not set!"));
		return;
	}
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		if (GridManagerRef->Grid.Num() == 0)
		{
			UE_LOG(LogTemp, Error, TEXT("Grid is still empty! Check GridManager settings."));
			return;
		}

		// --- A*
		/*
		double StartTime = FPlatformTime::Seconds();
		TArray<FGridNode*> PathA = AStarPathfinder::FindPath(GridManagerRef, StartPos, EndPos);
		double EndTime = FPlatformTime::Seconds();
		double Duration = EndTime - StartTime;
		
		UE_LOG(LogTemp, Warning, TEXT("A* finished in %f seconds"), Duration);
		*/
		// --- Dijkstra
		/*
		double StartTime = FPlatformTime::Seconds();
		TArray<FGridNode*> PathD = DijkstraPathfinder::FindPath(GridManagerRef, StartPos, EndPos);
		double EndTime = FPlatformTime::Seconds();
		double Duration = EndTime - StartTime;
		
		UE_LOG(LogTemp, Warning, TEXT("Dijkstra finished in %f seconds"), Duration);
		*/
		// --- BFS
		
		double StartTime = FPlatformTime::Seconds();
		TArray<FGridNode*> PathB = DijkstraPathfinder::FindPath(GridManagerRef, StartPos, EndPos);
		double EndTime = FPlatformTime::Seconds();
		double Duration = EndTime - StartTime;
		
		UE_LOG(LogTemp, Warning, TEXT("BFS finished in %f seconds"), Duration);
		
		
	}, 0.2f, false);
	
}




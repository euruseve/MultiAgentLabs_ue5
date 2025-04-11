// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/GridManager.h"


AGridManager::AGridManager()
{

}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();
	
	float NodeDiameter = NodeRadius * 2;
	FVector Origin = GridOrigin;

	Grid.SetNum(GridSizeY);
	for (int32 Y = 0; Y < GridSizeY; ++Y)
	{
		Grid[Y].SetNum(GridSizeX);
		for (int32 X = 0; X < GridSizeX; ++X)
		{
			FVector WorldPos = Origin + FVector(X * NodeDiameter, Y * NodeDiameter, 0.f);
			bool bWalkable = true;
			
			FHitResult Hit;
			FVector Start = WorldPos + FVector(0, 0, 50);
			FVector End = WorldPos - FVector(0, 0, 50);
			bWalkable = !GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic);

			Grid[Y][X] = FGridNode(WorldPos, X, Y, bWalkable);
			
			DrawDebugBox(GetWorld(), WorldPos, FVector(NodeRadius), bWalkable ? FColor::Green : FColor::Red, true, -1, 0, 2);
		}
	}
}

FGridNode* AGridManager::GetNodeFromWorldPosition(const FVector& WorldPos)
{
	if (Grid.Num() == 0 || Grid[0].Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Grid is not initialized!"));
		return nullptr;
	}

	float NodeDiameter = NodeRadius * 2;
	int32 X = FMath::Clamp(int32((WorldPos.X - GridOrigin.X) / NodeDiameter), 0, GridSizeX - 1);
	int32 Y = FMath::Clamp(int32((WorldPos.Y - GridOrigin.Y) / NodeDiameter), 0, GridSizeY - 1);

	return &Grid[Y][X];
}

TArray<FGridNode*> AGridManager::GetNeighborNodes(FGridNode* Node)
{
	TArray<FGridNode*> Neighbors;

	if (!Node || Grid.Num() == 0 || Grid[0].Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid node or grid!"));
		return Neighbors;
	}

	for (int32 OffsetY = -1; OffsetY <= 1; ++OffsetY)
	{
		for (int32 OffsetX = -1; OffsetX <= 1; ++OffsetX)
		{
			if (OffsetX == 0 && OffsetY == 0)
				continue;

			int32 CheckX = Node->GridX + OffsetX;
			int32 CheckY = Node->GridY + OffsetY;

			if (CheckX >= 0 && CheckX < GridSizeX && CheckY >= 0 && CheckY < GridSizeY)
			{
				Neighbors.Add(&Grid[CheckY][CheckX]);
			}
		}
	}

	return Neighbors;
}

void AGridManager::DrawDebugPath(const TArray<FGridNode*>& Path)
{
	for (int32 i = 0; i < Path.Num() - 1; ++i)
	{
		DrawDebugLine(GetWorld(), Path[i]->WorldPosition, Path[i + 1]->WorldPosition,
			FColor::Cyan, false, 5.0f, 0, 5.0f);

		
	}
}


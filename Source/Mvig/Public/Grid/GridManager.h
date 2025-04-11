// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

USTRUCT(BlueprintType)
struct FGridNode
{
	GENERATED_BODY()

	FGridNode()
		: WorldPosition(FVector::ZeroVector), GridX(0), GridY(0), bIsWalkable(true),
		  GCost(0), HCost(0), Parent(nullptr) {}

	FGridNode(FVector InPosition, int32 InX, int32 InY, bool bWalkable = true)
		: WorldPosition(InPosition), GridX(InX), GridY(InY), bIsWalkable(bWalkable),
		  GCost(0), HCost(0), Parent(nullptr) {}
	
	FVector WorldPosition;
	int32 GridX;
	int32 GridY;

	bool bIsWalkable;

	float GCost;
	float HCost;

	FORCEINLINE float FCost() const { return GCost + HCost; }

	FGridNode* Parent;
};


UCLASS(Blueprintable)
class MVIG_API AGridManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AGridManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	int32 GridSizeX = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	int32 GridSizeY = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	float NodeRadius = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	FVector GridOrigin = FVector::ZeroVector;

	TArray<TArray<FGridNode>> Grid;
	
protected:
	virtual void BeginPlay() override;

public:

	FGridNode* GetNodeFromWorldPosition(const FVector& WorldPos);
	
	TArray<FGridNode*> GetNeighborNodes(FGridNode* Node);
	
	void DrawDebugPath(const TArray<FGridNode*>& Path);

};

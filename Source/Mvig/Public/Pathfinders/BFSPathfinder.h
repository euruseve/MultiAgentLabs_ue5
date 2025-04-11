#pragma once

#include "CoreMinimal.h"
#include "../Grid/GridManager.h"

class BFSPathfinder
{
public:
	static TArray<FGridNode*> FindPath(AGridManager* Grid, FVector StartPos, FVector EndPos);
};

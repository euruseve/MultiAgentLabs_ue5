#include "AStarPathfinder.h"
#include "Containers/Queue.h"
#include "Algo/Reverse.h"

TArray<FGridNode*> AStarPathfinder::FindPath(AGridManager* Grid, FVector StartPos, FVector EndPos)
{
	FGridNode* StartNode = Grid->GetNodeFromWorldPosition(StartPos);
	FGridNode* EndNode = Grid->GetNodeFromWorldPosition(EndPos);

	TArray<FGridNode*> OpenSet;
	TSet<FGridNode*> ClosedSet;

	OpenSet.Add(StartNode);

	while (OpenSet.Num() > 0)
	{
		FGridNode* Current = OpenSet[0];
		for (FGridNode* Node : OpenSet)
		{
			if (Node->FCost() < Current->FCost() ||
				(Node->FCost() == Current->FCost() && Node->HCost < Current->HCost))
			{
				Current = Node;
			}
		}

		OpenSet.Remove(Current);
		ClosedSet.Add(Current);

		if (Current == EndNode)
		{
			TArray<FGridNode*> Path;
			FGridNode* Node = EndNode;
			while (Node != nullptr)
			{
				Path.Add(Node);
				Node = Node->Parent;
			}
			Algo::Reverse(Path);
			return Path;
		}

		for (FGridNode* Neighbor : Grid->GetNeighborNodes(Current))
		{
			if (!Neighbor->bIsWalkable || ClosedSet.Contains(Neighbor)) continue;

			float NewMovementCostToNeighbor = Current->GCost + FVector::Dist(Current->WorldPosition, Neighbor->WorldPosition);
			if (NewMovementCostToNeighbor < Neighbor->GCost || !OpenSet.Contains(Neighbor))
			{
				Neighbor->GCost = NewMovementCostToNeighbor;
				Neighbor->HCost = FVector::Dist(Neighbor->WorldPosition, EndNode->WorldPosition);
				Neighbor->Parent = Current;

				if (!OpenSet.Contains(Neighbor))
					OpenSet.Add(Neighbor);
			}
		}
	}

	return {};
}

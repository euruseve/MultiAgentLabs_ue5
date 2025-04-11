#include "DijkstraPathfinder.h"
#include "Algo/Reverse.h"

TArray<FGridNode*> DijkstraPathfinder::FindPath(AGridManager* Grid, FVector StartPos, FVector EndPos)
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
			if (Node->GCost < Current->GCost)
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

			float NewCost = Current->GCost + FVector::Dist(Current->WorldPosition, Neighbor->WorldPosition);
			if (NewCost < Neighbor->GCost || !OpenSet.Contains(Neighbor))
			{
				Neighbor->GCost = NewCost;
				Neighbor->Parent = Current;

				if (!OpenSet.Contains(Neighbor))
					OpenSet.Add(Neighbor);
			}
		}
	}

	return {};
}

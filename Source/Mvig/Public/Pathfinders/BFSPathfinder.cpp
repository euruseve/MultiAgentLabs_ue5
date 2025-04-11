#include "BFSPathfinder.h"
#include "Containers/Queue.h"
#include "Algo/Reverse.h"

TArray<FGridNode*> BFSPathfinder::FindPath(AGridManager* Grid, FVector StartPos, FVector EndPos)
{
	
	if (!Grid) return {};

	FGridNode* StartNode = Grid->GetNodeFromWorldPosition(StartPos);
	FGridNode* EndNode = Grid->GetNodeFromWorldPosition(EndPos);

	if (!StartNode || !EndNode)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid start or end node!"));
		return {};
	}

	
	if (!StartNode || !EndNode) return {};

	TQueue<FGridNode*> Queue;
	TMap<FGridNode*, FGridNode*> CameFrom;

	Queue.Enqueue(StartNode);
	CameFrom.Add(StartNode, nullptr);

	while (!Queue.IsEmpty())
	{
		FGridNode* Current;
		Queue.Dequeue(Current);

		if (Current == EndNode)
		{
			TArray<FGridNode*> Path;
			FGridNode* Node = EndNode;
			while (Node != nullptr)
			{
				Path.Add(Node);
				Node = CameFrom[Node];
			}
			Algo::Reverse(Path);
			return Path;
		}

		for (FGridNode* Neighbor : Grid->GetNeighborNodes(Current))
		{
			if (!Neighbor->bIsWalkable || CameFrom.Contains(Neighbor)) continue;

			CameFrom.Add(Neighbor, Current);
			Queue.Enqueue(Neighbor);
		}
	}
	return {};
}

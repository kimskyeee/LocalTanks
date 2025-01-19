#pragma once

#include <queue>
#include <stack>

#include "CoreMinimal.h"
#include "FTileNode.h"
#include "IO/IoPriorityQueue.h"
#include "UObject/Object.h"
#include "UAStarPathFinding.generated.h"

UCLASS()
class UAStarPathFinding : public UObject
{
	GENERATED_BODY()
public:
	UAStarPathFinding();

	UFUNCTION(BlueprintCallable, Category = "PathFinding")
	void FindPath(FVector Start, FVector End, TArray<FVector>& OutPath);
	UFUNCTION(BlueprintCallable, Category = "PathFinding")
	void ResetField();
	UFUNCTION(BlueprintCallable, Category = "PathFinding")
	bool IsWall(const FVector& Goal);

protected:
	FTileNode* GetTileNode(FVector Location);
	void DrawTileMap();
	void DrawWalls();
	// TileMap[y][x]

	// 언리얼 배열 너무 구지다.. ㅠ TArray가 원하는 방식대로 동작하지 않음
	std::vector<std::vector<FTileNode>> TileMap;
	// 언리얼 제공 우선순위 큐가 없어서 STL 우선순위 큐 사용
	std::priority_queue<FTileNode*, std::vector<FTileNode*>, FTileNodeCompareAscending> OpenList;
	std::priority_queue<FTileNode*, std::vector<FTileNode*>, FTileNodeCompareAscending> ClosedList; // 캐시로 대체 가능
	// std::priority_queue<FTileNode*, std::vector<FTileNode*>, FTileNodeCompareAscending> ParentList;
	std::stack<FTileNode*> ParentPathStack;

	float EntireTileWidth;
	float EntireTileHeight;
	float TileWidth = 2000;
	float TileHeight = 2000;
	int32 TileXCount = 21;
	int32 TileYCount = 21;

	bool bCanSearchDiagonal = true;
	bool bCantCrossWall = true;
};

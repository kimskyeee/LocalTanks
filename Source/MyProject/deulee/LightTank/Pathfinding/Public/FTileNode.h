#pragma once

#include "CoreMinimal.h"
#include "FTileNode.generated.h"

USTRUCT()
struct FTileNode
{
	GENERATED_BODY()

	UPROPERTY()
	int32 XIndex;
	UPROPERTY()
	int32 YIndex;
	UPROPERTY()
	float XCenter;
	UPROPERTY()
	float YCenter;
	
	UPROPERTY()
	float Width;
	UPROPERTY()
	float Height;

	UPROPERTY()
	bool bIsWall;
	UPROPERTY()
	bool bIsVisited;
	
	UPROPERTY()
	float HCost; // 장애물 고려 안한 목표까지의 거리
	UPROPERTY()
	float GCost; // 누적 이동 비용
	UPROPERTY()
	float FCost; // HCost + GCost

	UPROPERTY()
	float DistanceFromCenter;
	
	FTileNode* ParentNode;

	FTileNode();
	FTileNode(int32 X, int32 Y, float XCenter, float YCenter, float Width, float Height, bool bIsWall);
	FTileNode(const FTileNode& Other);
	void Reset();

	// FCost가 낮은게 우선순위가 높다.
	bool operator<(const FTileNode& Other) const
	{
		return FCost > Other.FCost;
	}
};

// FCost가 낮은게 우선순위가 높다.
struct FTileNodeCompareAscending
{
	bool operator()(const FTileNode* LHS, const FTileNode* RHS) const
	{
		return LHS->FCost > RHS->FCost;
	}
};

// FCost가 높은게 우선순위가 높다.
struct FTileNodeCompareDescending
{
	bool operator()(const FTileNode* LHS, const FTileNode* RHS) const
	{
		return LHS->FCost < RHS->FCost;
	}
};
#include "FTileNode.h"

FTileNode::FTileNode(): XIndex(0), YIndex(0), XCenter(0), YCenter(0), Width(0), Height(0), bIsWall(false),
                        bIsVisited(false),
                        HCost(0),
                        GCost(0),
                        FCost(0),
                        DistanceFromCenter(0),
                        ParentNode(nullptr)
{
}

FTileNode::FTileNode(int32 X, int32 Y, float XCenter, float YCenter, float Width, float Height, bool bIsWall):
	XIndex(X),
	YIndex(Y),
	XCenter(XCenter),
	YCenter(YCenter),
	Width(Width),
	Height(Height),
	bIsWall(bIsWall),
	bIsVisited(false),
	HCost(0),
	GCost(0),
	FCost(0),
	DistanceFromCenter(0),
	ParentNode(nullptr)
{
}

FTileNode::FTileNode(const FTileNode& Other)
	: XIndex(Other.XIndex),
	  YIndex(Other.YIndex),
	  XCenter(Other.XCenter),
	  YCenter(Other.YCenter),
	  Width(Other.Width),
	  Height(Other.Height),
	  bIsWall(Other.bIsWall),
	  bIsVisited(Other.bIsVisited),
	  HCost(Other.HCost),
	  GCost(Other.GCost),
	  FCost(Other.FCost),
	  DistanceFromCenter(Other.DistanceFromCenter),
	  ParentNode(Other.ParentNode) // 일부로 얕은 복사를 함. 깊은 복사를 할 시 메모리 낭비가 너무 심함
{
}

void FTileNode::Reset()
{
	HCost = 0;
	GCost = 0;
	FCost = 0;
	bIsVisited = false;
	ParentNode = nullptr;
}

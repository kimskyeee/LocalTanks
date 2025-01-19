#include "UAStarPathFinding.h"

#include "FastLogger.h"
#include "NetworkMessage.h"
#include "GenericPlatform/GenericPlatformChunkInstall.h"

UAStarPathFinding::UAStarPathFinding()
{
	EntireTileHeight = TileHeight * TileYCount;
	EntireTileWidth = TileWidth * TileXCount;
	TileMap.resize(TileYCount);
	for (int32 i = 0; i < TileYCount; i++)
	{
		TileMap[i].resize(TileXCount);
	}
	// TileMap 초기화
	float TileWidthOffset = EntireTileWidth / 2;
	float TileHeightOffset = EntireTileHeight / 2;

	// i : x, j : y
	// TileMap[y][x]
	for (int32 i = 0; i < TileXCount; i++)
	{
		for (int32 j = 0; j < TileYCount; j++)
		{
			// TileMap[i][j] 초기화
			// 중심 좌표는 EntireTileWidht / 2, EntireTileHeight / 2
			float XCenter = j * TileWidth + TileWidth / 2 - TileWidthOffset;
			float YCenter = i * TileHeight + TileHeight / 2 - TileHeightOffset;
			FTileNode Node(j, i, XCenter, YCenter, TileWidth, TileHeight, 0);
			TileMap[i][j] = Node;
		}
	}

	
	// 벽 설정 로직 필요
	FTileNode* WallNode = GetTileNode({9707, 5727, 0});
	FTileNode* WallNode1 = GetTileNode({9707, 3677.655245, 0});
	FTileNode* WallNode2 = GetTileNode({7257.383105, 3677.655245, 0});
	FTileNode* WallNode3 = GetTileNode({7257.383105, 5977.655245, 0});
	
	FVector Wall1 = { WallNode->XCenter, WallNode->YCenter, 0 };
	FVector Wall2 = { WallNode1->XCenter, WallNode1->YCenter, 0 };
	FVector Wall3 = { WallNode2->XCenter, WallNode2->YCenter, 0 };
	FVector Wall4 = { WallNode3->XCenter, WallNode3->YCenter, 0 };
	
	// WallNode->bIsWall = 1;
	// WallNode1->bIsWall = 1;
	// WallNode2->bIsWall = 1;
	// WallNode3->bIsWall = 1;
	
	// (X=12707.383105,Y=7147.655245,Z=0.000001)
	
}

void UAStarPathFinding::FindPath(FVector Start, FVector End, TArray<FVector>& OutPath)
{
	DrawTileMap();
	// 시작 노드와 끝 노드를 가져온다.
	FTileNode* StartNode = GetTileNode(Start);
	FTileNode* EndNode = GetTileNode(End);

	if (StartNode == nullptr || EndNode == nullptr)
	{
		FFastLogger::LogScreen(FColor::Red, TEXT("StartNode or EndNode is nullptr"));
		return;
	}

	StartNode->bIsVisited = true;
	StartNode->GCost = 0;
	// 어차피 타일의 크기는 같으므로 Unit Cost는 1로 설정
	StartNode->HCost = FMath::Abs(StartNode->XIndex - EndNode->XIndex) + FMath::Abs(StartNode->YIndex - EndNode->YIndex);
	StartNode->FCost = StartNode->GCost + StartNode->HCost;
	StartNode->ParentNode = nullptr;
	OpenList.push(StartNode);

	// 직선 이동
	int32 XOffsetLinear[] = { 0, 1, 0, -1 };
	int32 YOffsetLinear[] = { 1, 0, -1, 0 };
	// 대각선 이동
	int32 XOffsetDiagonal[] = { 1, 1, -1, -1 };
	int32 YOffsetDiagonal[] = { 1, -1, -1, 1 };

	while (!OpenList.empty())
	{
		FTileNode* CurrentNode = OpenList.top();
		OpenList.pop();

		int32 CurrentX = CurrentNode->XIndex;
		int32 CurrentY = CurrentNode->YIndex;

		// 목표지점에 도착하면 종료 후 경로를 ParentNode를 따라가며 ParentPath에 저장
		if (CurrentX == EndNode->XIndex && CurrentY == EndNode->YIndex)
		{
			FTileNode* Node = CurrentNode;
			// Skip StartNode
			while (Node->ParentNode != nullptr)
			{
				ParentPathStack.push(Node);
				Node = Node->ParentNode;
			}

			if (ParentPathStack.empty())
			{
				ParentPathStack.push(Node);
			}
			break;
		}

		// 직선 이동 체크 : 4방향
		for (int32 i = 0; i < 4; i++)
		{
			int32 NextX = CurrentX + XOffsetLinear[i];
			int32 NextY = CurrentY + YOffsetLinear[i];

			// 범위 체크
			if (NextX < 0 || NextX >= TileXCount || NextY < 0 || NextY >= TileYCount)
			{
				continue;
			}
			// 벽 체크
			if (TileMap[NextY][NextX].bIsWall)
			{
				continue;
			}
			// 방문 체크
			if (TileMap[NextY][NextX].bIsVisited)
			{
				continue;
			}

			TileMap[NextY][NextX].bIsVisited = true;
			TileMap[NextY][NextX].GCost = CurrentNode->GCost + 1;
			TileMap[NextY][NextX].HCost = FMath::Abs(NextX - EndNode->XIndex) + FMath::Abs(NextY - EndNode->YIndex);
			TileMap[NextY][NextX].FCost = TileMap[NextY][NextX].GCost + TileMap[NextY][NextX].HCost;
			TileMap[NextY][NextX].ParentNode = CurrentNode;
			OpenList.push(&TileMap[NextY][NextX]);
		}

		// 대각선 이동 체크 : 4방향
		for (int32 i = 0; i < 4; i++)
		{
			int32 NextX = CurrentX + XOffsetDiagonal[i];
			int32 NextY = CurrentY + YOffsetDiagonal[i];

			if (!bCanSearchDiagonal)
			{
				continue;
			}
			// 범위 체크
			if (NextX < 0 || NextX >= TileXCount || NextY < 0 || NextY >= TileYCount)
			{
				continue;
			}
			// 벽 체크
			if (TileMap[NextY][NextX].bIsWall)
			{
				continue;
			}
			// 방문 체크
			if (TileMap[NextY][NextX].bIsVisited)
			{
				continue;
			}
			// 벽을 통과할 수 없는 경우
			if (TileMap[CurrentY][NextX].bIsWall && TileMap[NextY][CurrentX].bIsWall)
			{
				continue;
			}
			// 벽이 하나라도 있으면 대각선 이동 불가
			if (bCantCrossWall && TileMap[CurrentY][NextX].bIsWall || TileMap[NextY][CurrentX].bIsWall)
			{
				continue;
			}

			TileMap[NextY][NextX].bIsVisited = true;
			TileMap[NextY][NextX].GCost = CurrentNode->GCost + 1.4; // 대각선 이동 비용 : 루트 2
			TileMap[NextY][NextX].HCost = FMath::Abs(NextX - EndNode->XIndex) + FMath::Abs(NextY - EndNode->YIndex);
			TileMap[NextY][NextX].FCost = TileMap[NextY][NextX].GCost + TileMap[NextY][NextX].HCost;
			TileMap[NextY][NextX].ParentNode = CurrentNode;
			OpenList.push(&TileMap[NextY][NextX]);
		}
	}

	// 경로를 OutPath에 저장
	while (!ParentPathStack.empty())
	{
		FTileNode* Node = ParentPathStack.top();
		ParentPathStack.pop();
		OutPath.Add(FVector(Node->XCenter, Node->YCenter, 0));

		// 디버그 라인 시작과 끝 위치 계산
		FVector LineStart({Node->XCenter, Node->YCenter, 0});
		FVector LineEnd = LineStart + FVector(0, 0, 100000); // Z축으로 100 단위 위로

		DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Green, false, 100000000.f, 0, 10);
	}
}

void UAStarPathFinding::ResetField()
{
	for (int32 i = 0; i < TileYCount; i++)
	{
		for (int32 j = 0; j < TileXCount; j++)
		{
			TileMap[i][j].Reset();
		}
	}
	while (!OpenList.empty())
	{
		OpenList.pop();
	}
	while (!ParentPathStack.empty())
	{
		ParentPathStack.pop();
	}
	while (!ClosedList.empty())
	{
		ClosedList.pop();
	}
	
}

bool UAStarPathFinding::IsWall(const FVector& Goal)
{
	FTileNode* TileNode = GetTileNode(Goal);
	if (TileNode == nullptr)
	{
		return false;
	}
	return TileNode->bIsWall;
}

FTileNode* UAStarPathFinding::GetTileNode(FVector Location)
{
	// TileMap[y][x]
	// Local의 좌표가 -로 들어올 수도 있다.
	FVector Local = Location + FVector(EntireTileWidth / 2, EntireTileHeight / 2, 0);
	int32 YIndex = Local.Y / TileHeight;
	int32 XIndex = Local.X / TileWidth;
	if (YIndex < 0 || YIndex >= TileYCount || XIndex < 0 || XIndex >= TileXCount)
	{
		FFastLogger::LogScreen(FColor::Red, TEXT("Out of Range"));
		return nullptr;
	}
	return &TileMap[YIndex][XIndex];
}

void UAStarPathFinding::DrawTileMap()
{
	if (TileMap.empty() || TileMap[0].empty()) return;

	// 기존 디버그 라인을 모두 제거
	FlushPersistentDebugLines(GetWorld());
	
	int32 RowCount = TileMap.size();       // TileMap의 행(row) 수
	int32 ColumnCount = TileMap[0].size(); // TileMap의 열(column) 수

	// 행 경계선을 그린다
	for (int32 i = 0; i <= RowCount; i++)
	{
		float YStart = (i == 0) ? TileMap[0][0].YCenter - TileHeight / 2 : TileMap[i - 1][0].YCenter + TileHeight / 2;
		FVector LineStart(TileMap[0][0].XCenter - TileWidth / 2, YStart, 1000);
		FVector LineEnd(TileMap[0][ColumnCount - 1].XCenter + TileWidth / 2, YStart, 1000);

		DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Blue, true, 10000000.0f, 0, 10.0f);
	}

	// 열 경계선을 그린다
	for (int32 j = 0; j <= ColumnCount; j++)
	{
		float XStart = (j == 0) ? TileMap[0][0].XCenter - TileWidth / 2 : TileMap[0][j - 1].XCenter + TileWidth / 2;
		FVector LineStart(XStart, TileMap[0][0].YCenter - TileHeight / 2, 1000);
		FVector LineEnd(XStart, TileMap[RowCount - 1][0].YCenter + TileHeight / 2, 1000);

		DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Blue, true, 10000000.0f, 0, 10.0f);
	}

	DrawWalls();
}

void UAStarPathFinding::DrawWalls()
{
	for (int32 i = 0; i < TileYCount; i++)
	{
		for (int32 j = 0; j < TileXCount; j++)
		{
			if (TileMap[i][j].bIsWall)
			{
				FVector WallCenter = FVector(TileMap[i][j].XCenter, TileMap[i][j].YCenter, 0);
				DrawDebugLine(GetWorld(), WallCenter, WallCenter + FVector(0, 0, 100000000.f), FColor::Red, false, 100000000.f, 0, 10);
			}
		}
	}
}




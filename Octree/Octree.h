#ifndef _OCTREE_H
#define _OCTREE_H

#include "Main.h"
#include "3ds.h"


extern int g_MaxTriangles;

extern int g_MaxSubdivisions;

extern int g_EndNodeCount;

extern int g_TotalNodesDrawn;

extern bool g_bLighting;

extern bool g_RenderMode;

enum eOctreeNodes
{
	TOP_LEFT_FRONT,			// 0
	TOP_LEFT_BACK,			// 1
	TOP_RIGHT_BACK,			// 2
	TOP_RIGHT_FRONT,        // ~ ~ ~
	BOTTOM_LEFT_FRONT,
	BOTTOM_LEFT_BACK,
	BOTTOM_RIGHT_BACK,
	BOTTOM_RIGHT_FRONT
};

struct tFaceList
{
	vector<bool> pFaceList;	

	int totalFaceCount;
};

class CDebug 
{

public:
	void AddDebugLine(CVector3 vPoint1, CVector3 vPoint2);

	void AddDebugRectangle(CVector3 vCenter, float width, float height, float depth);

	void RenderDebugLines();		

	void Clear();					

private:
	vector<CVector3> m_vLines;		
};


class COctree
{

public:

	COctree();
	~COctree();

	CVector3 GetCenter() {	 return m_vCenter;	}

	int GetTriangleCount()  {   return m_TriangleCount;	}

	float GetWidth() {	 return m_Width;	}

	bool IsSubDivided()  {   return m_bSubDivided;	}

	void GetSceneDimensions(t3DModel *pWorld);

	int GetSceneTriangleCount(t3DModel *pWorld);

	int GetDisplayListID()		{   return m_DisplayListID;		}

	void SetDisplayListID(int displayListID)	{	m_DisplayListID = displayListID;  }

	void AddObjectIndexToList(int index);

	CVector3 GetNewNodeCenter(CVector3 vCenter, float width, int nodeID);

	void CreateNode(t3DModel *pWorld, int numberOfTriangles, CVector3 vCenter, float width);

	void CreateNewNode(t3DModel *pWorld,	vector<tFaceList> pList, int triangleCount,
				  	   CVector3 vCenter,	float width,			 int nodeID);

	// 마지막 노드의 폴리곤을 정렬한다. 
	void AssignTrianglesToNode(t3DModel *pWorld, int numberOfTriangles);

	// 마지막으로 생성된 노드에서 다시 옥트리로 분할할때의 노드 생성
	void DrawOctree(COctree *pNode, t3DModel *pRootWorld);

	// 옥트리 안의 모든 마지막 노드를 위한 디스플레이 리스트 ID를 생성한다.
	void CreateDisplayList(COctree *pNode, t3DModel *pRootWorld, int displayListOffset);


private:

	bool m_bSubDivided;

	float m_Width;

	int m_TriangleCount;

	CVector3 m_vCenter;

	// all the scene information (verts, normals, texture info, etc..) 
	t3DModel *m_pWorld;

	// This stores the indices into the original model's object list
	vector<int>	m_pObjectList;

	// 현재 노드의 List ID
	int m_DisplayListID;

	// 현재 노드안의 8개의 노드 ㅈ
	COctree *m_pOctreeNodes[8];	
};


CVector3 Cross(CVector3 vVector1, CVector3 vVector2);

float Magnitude(CVector3 vNormal);

CVector3 Normalize(CVector3 vVector);


#endif


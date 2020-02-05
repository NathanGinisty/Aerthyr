#ifndef MAPLOADER_H
#define MAPLOADER_H

#include "../stdafx.h"
#include "../MathsElements/MathElements.h"

typedef struct ImageTile ImageTile;


struct ImageTile
{
	sfVector2f pos;
	int layer;
	int imageID;
	struct ImageTile* ptrNext;
	struct ImageTile* prtPrevious;
};

typedef struct
{
	ImageTile* first;
	ImageTile* last;
}ChainImageTile;

typedef struct
{
	int min;
	int max;
}RandBounds;

typedef struct
{
	SegmentLine* line;
	char nbLine;
}MapPrefab;

typedef struct
{
	MapPrefab *prefabs;
	int nbPrefabs;
}CollisionMap;

typedef struct
{
	char mapNum;
	char instancePos;
}MapSort;

typedef struct
{
	int mapNb;
	MapSort* mapArray;
}MapGenerated;

typedef struct
{
	int width;
	int height;
	char name[50];

	int*** assetBack;
	int nbAssetBack;

	int** colliderID;

	int** enemyTile;

	int*** assetFront;
	int nbAssetFront;

	ChainImageTile backImage;
	ChainImageTile frontImage;

}TiledMap;

typedef struct
{
	TiledMap *maps;
	char mapsNb;
	int actualMap;
}InstanceMaps;


typedef struct
{
	int iMin;
	int iMax;
	int jMin;
	int jMax;
}OptiRender;



// ----- FUNCTION -----//

CollisionMap MapCollisionLoader(char area);
void GenerateMapPattern(MapGenerated* tmpTest, int min, int max, int totalMap);
InstanceMaps ReadMap(char area);

#endif //!MAPLOADER_H
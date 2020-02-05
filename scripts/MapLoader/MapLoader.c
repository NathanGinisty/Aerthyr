/*
******* ERREUR / PROBLEME ********

------ 17 / 01 / 2018 ------
Fonction ReadMap :
L.265 : probleme de free de tableau result.mapArray

******* A FAIRE ********

- afficher la map a partir du fichier tiled

*/
#include "mapLoader.h"

typedef enum
{
	AssetBack,
	AssetFront,
	Enemy,
	Image,
	Collider
}LoadLayerType;

typedef enum
{
	back,
	front
}ImageTileType;


//#define DEBUG_LOAD_MAP 

// ---------------------------------------------------------------------------------------------
void GetPrefabsNb(CollisionMap* map, FILE* file)
{
	sfBool isFirst = sfTrue;
	int numBlock = 0;

	char fileName[100];
	sprintf(fileName, "Maps/Area%d/Tileset_Collider.png", 1);
	map->nbPrefabs = 0;
	sfTexture* texture = sfTexture_createFromFile(fileName, NULL);


	int sizeMapX = 0;
	int sizeMapY = 0;

	char string[500] = { 0 };
	// Get size X of SVG
	while (sizeMapX == 0 && !feof(file))
	{
		fgets(string, 500, file);
		if (strstr(string, "width") != NULL)
		{
			sscanf(strstr(string, "width") + 7, "%d", &sizeMapX);
		}
	}
	rewind(file);

	// Get size Y of SVG
	while (sizeMapY == 0 && !feof(file))
	{
		fgets(string, 500, file);
		if (strstr(string, "height") != NULL)
		{
			sscanf(strstr(string, "height") + 8, "%d", &sizeMapY);
		}
	}
	rewind(file);

	map->nbPrefabs = sizeMapX / SIZE_TILE * sizeMapY / SIZE_TILE;

	int *tmpLine = (int*)malloc(map->nbPrefabs * sizeof(int));
	for (int i = 0; i < map->nbPrefabs; i++)
	{
		tmpLine[i] = 0;
	}

	do
	{
		char* buffer = calloc(800, sizeof(char));

		fscanf(file, "%s", buffer);

		if (strstr(buffer, "line") != NULL)
		{
			fscanf(file, " id=\"c_%d", &numBlock);
			tmpLine[numBlock] += 1;
		}
		free(buffer);
	} while (!feof(file));

	map->prefabs = (MapPrefab*)malloc(map->nbPrefabs * sizeof(MapPrefab));

	for (int i = 0; i < map->nbPrefabs; i++)
	{
		map->prefabs[i].nbLine = tmpLine[i];
		map->prefabs[i].line = (SegmentLine*)malloc(tmpLine[i] * sizeof(SegmentLine));
	}
}

// ---------------------------------------------------------------------------------------------
float GetMapPoint(char* balise, FILE* file)
{
	char* ptrEnd;
	char tmpBuffer[100];
	char bufferValue[50];
	char index = 4;
	char indexValue = 0;
	do
	{
		fscanf(file, "%s", tmpBuffer);

	} while (strstr(tmpBuffer, balise) == NULL);

	while (tmpBuffer[index] != '"')
	{
		bufferValue[indexValue] = tmpBuffer[index];
		index++;
		indexValue++;
	}
	bufferValue[indexValue] = '\0';
	//printf("find :%f\n", strtof(bufferValue, &ptrEnd));
	//printf("find :%s\n", bufferValue);
	return strtof(bufferValue, &ptrEnd);
}

// ---------------------------------------------------------------------------------------------
SegmentLine GetMapLine(FILE* file, sfVector2i pos)
{
	SegmentLine tmpLine;
	//printf("PointA.x-> ");
	tmpLine.pointA.x = GetMapPoint("x1=", file) - pos.x * SIZE_TILE;
	//printf("PointA.y-> ");
	tmpLine.pointA.y = GetMapPoint("y1=", file) - pos.y * SIZE_TILE;
	//printf("PointB.x-> ");
	tmpLine.pointB.x = GetMapPoint("x2=", file) - pos.x * SIZE_TILE;
	//printf("PointB.y-> ");
	tmpLine.pointB.y = GetMapPoint("y2=", file) - pos.y * SIZE_TILE;

	//printf("A.x:%f\n", tmpLine.pointA.x);
	//printf("A.y:%f\n", tmpLine.pointA.y);
	//printf("B.x:%f\n", tmpLine.pointB.x);
	//printf("B.y:%f\n", tmpLine.pointB.y);

	tmpLine.dir = tmpLine.pointB;
	tmpLine.dir.x -= tmpLine.pointA.x;
	tmpLine.dir.y -= tmpLine.pointA.y;
	tmpLine.dir = Normalize(tmpLine.dir);

	return tmpLine;
}

// ---------------------------------------------------------------------------------------------
CollisionMap MapCollisionLoader(char area)
{
	CollisionMap map;

	FILE* mapCollisionFile;

	int prefabIndex = 0;
	int lineIndex = 0;

	char fileName[100];
	sprintf(fileName, "Maps/Area%d/MapCollisionFile.svg", area + 1);
	mapCollisionFile = fopen(fileName, "rt");

	if (mapCollisionFile == NULL)
	{
		printf("ERROR FILE LOADING : MapCollisonFile.svg\n");
		exit(EXIT_FAILURE);
	}

	int sizeMapX = 0;
	char string[500] = { 0 };
	// Get size X of SVG
	while (sizeMapX == 0 && !feof(mapCollisionFile))
	{
		fgets(string, 500, mapCollisionFile);
		if (strstr(string, "width") != NULL)
		{
			sscanf(strstr(string, "width") + 7, "%d", &sizeMapX);
			sizeMapX /= SIZE_TILE;
		}
	}
	rewind(mapCollisionFile);

	GetPrefabsNb(&map, mapCollisionFile);
	printf("DEBUG PASS 2\n");

	rewind(mapCollisionFile);

	do
	{
		char* buffer[800] = { 0 };

		fscanf(mapCollisionFile, "%s", buffer);

		if (strstr(buffer, "line") != NULL)
		{
			fscanf(mapCollisionFile, " id=\"c_%d_%d", &prefabIndex, &lineIndex);

			//printf("map.prefabs[%d].line[%d] :\n", prefabIndex, lineIndex - 1);
			map.prefabs[prefabIndex].line[lineIndex - 1] = GetMapLine(mapCollisionFile, (sfVector2i) { prefabIndex % sizeMapX, prefabIndex / sizeMapX });

		}
	} while (!feof(mapCollisionFile));
	printf("DEBUG PASS 3\n");

	fclose(mapCollisionFile);
	return map;
}

// ---------------------------------------------------------------------------------------------
int GetAreaMapNumber(FILE* f)
{
	char buff[50];
	int nbMaps = 0;
	do
	{
		fscanf(f, "%s", buff);
		if (strcmp(buff, "name:") == 0)
		{
			nbMaps += 1;
		}
	} while (!feof(f));

	return nbMaps;
}

// ---------------------------------------------------------------------------------------------
void GenerateMapPattern(MapGenerated* tmpTest, int min, int max, int totalMap)
{
	int randModulo = max - min + 1;
	char nbOfMaps = min + 1 + rand() % randModulo;
	int *possibilities = (int*)malloc(totalMap * sizeof(int));
	int nbPossibilities = totalMap;
	tmpTest->mapNb = nbOfMaps;
	tmpTest->mapArray = (MapSort*)malloc(nbOfMaps * sizeof(MapSort));
	for (int i = 0; i < totalMap; i++)
	{
		possibilities[i] = i;
	}

	for (int i = 0; i < nbOfMaps && nbPossibilities > 0; i++)
	{
		int randResult = rand() % nbPossibilities;
		tmpTest->mapArray[i].mapNum = possibilities[randResult];
		printf("i value : %d | map id : %d\n", i, possibilities[randResult]);
		tmpTest->mapArray[i].instancePos = i;
		possibilities[randResult] = possibilities[nbPossibilities - 1];
		nbPossibilities--;
	}

	free(possibilities);
	possibilities = NULL;
}

// ---------------------------------------------------------------------------------------------
int QsortMapInstance(const void* a, const void* b)
{
	MapSort* pa = (MapSort*)a;
	MapSort* pb = (MapSort*)b;
	return pa->mapNum - pb->mapNum;
}

// ---------------------------------------------------------------------------------------------
int CorrectiveIdTile(int tmpID, int nbTileSet, int firstgid[])
{
	// for each tileset
	// corrective of .tmx ID

	for (int t = nbTileSet - 1; t >= 0; t--)
	{
		// check if ID == 0 to set MapID to -1 (noBlock);
		if (!tmpID)
		{
			return -1;
		}
		else if (tmpID >= firstgid[t])
		{
			return tmpID - firstgid[t];
		}
	}
	return -1;
}

// ---------------------------------------------------------------------------------------------
ChainImageTile InitChainImageTile()
{
	ChainImageTile tmpChain;

	tmpChain.first = NULL;
	tmpChain.last = NULL;

	return tmpChain;
}

// ---------------------------------------------------------------------------------------------
void AddImageTileFront(ChainImageTile* chain, sfVector2f pos, int layer, int imageID)
{
	struct ImageTile* ImageTile = (struct ImageTile*) calloc(sizeof(struct ImageTile), 1);

	ImageTile->pos = pos;
	ImageTile->layer = layer;
	ImageTile->imageID = imageID;


	ImageTile->ptrNext = chain->first;
	ImageTile->prtPrevious = NULL;

	if (chain->first != NULL)
	{
		chain->first->prtPrevious = ImageTile;
	}
	if (chain->first == NULL)
	{
		chain->last = ImageTile;
	}

	chain->first = ImageTile;
}

// ---------------------------------------------------------------------------------------------
int GetImageID(char* name)
{
	int id = 0;
	char string[500] = { 0 };
	int actualIndex = 0;
	int actualIndexAnim = 0;

	FILE* dataFile = fopen("Assets/TileImage/tileImage.dat", "r");

	if (dataFile == NULL)
	{
		printf("ERROR \"tileImage.dat\" CORRUPTED\n");
		exit(0);
	}

	while (!feof(dataFile))
	{
		fgets(string, 500, dataFile);

		if (strstr(string, "<Asset") != NULL)
		{
			if (strstr(string, name))
			{
				fclose(dataFile);
				return actualIndex;
			}
			actualIndex++;
		}
	}
	fclose(dataFile);

	printf("ERROR Image not found in tileimage.dat .\nCheck if you set the image in data bank\n");
	exit(0);
}

// ---------------------------------------------------------------------------------------------
TiledMap ReadTiled(char* nameMap, char area)
{
	TiledMap map;
	char fileName[100];

	sprintf(fileName, "Maps/Area%d/MapFiles/%s.map", area, nameMap);
	FILE* mapFile = fopen(fileName, "r");

	char string[1500] = { 0 };
	char name[200] = { 0 };
	int firstgid[100];
	int nbTileSet = 0;
	int actualAsset = 0;
	int tmpID = 0;
	LoadLayerType actualType;


	if (mapFile == NULL)
	{
		printf("ERROR LOAD MAP Maps / Area%d / MapFiles / %s.map", area, nameMap);
		exit(0);
	}

	//Init 

	map.nbAssetBack = 0;
	map.nbAssetFront = 0;

	// GET NB LAYER AND TILESET INFO
	while (!feof(mapFile))
	{
		fgets(string, 1500, mapFile);

		// Get Map Size
		if (strstr(string, "<map") != NULL)
		{
			sscanf(strstr(string, "width=\"") + strlen("width=\""), "%d", &map.width);
			sscanf(strstr(string, "height=\"") + strlen("height=\""), "%d", &map.height);
		}

		//getNb TileSet
		if (strstr(string, "<tileset") != NULL)
		{
			sscanf(strstr(string, "firstgid=\"") + strlen("firstgid=\""), "%d", &firstgid[nbTileSet]);
			nbTileSet++;
		}

		// Get Nb Layer 
		if (strstr(string, "<layer") != NULL)
		{
			char * posInfoNb = NULL;
			if (posInfoNb = strstr(string, "AssetBack_"))
			{
				map.nbAssetBack++;
			}
			else if (posInfoNb = strstr(string, "AssetFront_"))
			{
				map.nbAssetFront += 1;
			}
		}
	}


#pragma region //Malloc all Array
	//ASSETBACK
	map.assetBack = (int***)malloc(map.nbAssetBack * sizeof(int**));
	for (int i = 0; i < map.nbAssetBack; i++)
	{
		map.assetBack[i] = (int**)malloc(map.width * sizeof(int*));
		for (int j = 0; j < map.width; j++)
		{
			map.assetBack[i][j] = (int*)malloc(map.height * sizeof(int));
		}
	}

	//ASSETFRONT
	map.assetFront = (int***)malloc(map.nbAssetFront * sizeof(int**));
	for (int i = 0; i < map.nbAssetFront; i++)
	{
		map.assetFront[i] = (int**)malloc(map.width * sizeof(int*));
		for (int j = 0; j < map.width; j++)
		{
			map.assetFront[i][j] = (int*)malloc(map.height * sizeof(int));
		}
	}

	//colliderID
	map.colliderID = (int**)malloc(map.width * sizeof(int*));
	for (int i = 0; i < map.width; i++)
	{
		map.colliderID[i] = (int*)malloc(map.height * sizeof(int));
	}

	//enemyTile
	map.enemyTile = (int**)malloc(map.width * sizeof(int*));
	for (int i = 0; i < map.width; i++)
	{
		map.enemyTile[i] = (int*)malloc(map.height * sizeof(int));
	}

	//// Init chainlist Image
	map.backImage = InitChainImageTile();
	map.frontImage = InitChainImageTile();
#pragma endregion

	// REWIND FILE 
	rewind(mapFile);

	// READ MAP DATA

	while (!feof(mapFile))
	{
		fgets(string, 1500, mapFile);

		//Enter in Read Data MODE if string contain "<layer"
		if (strstr(string, "<layer") != NULL)
		{
			char * posInfoNb = NULL;
			// get Name and Nb
			if (posInfoNb = strstr(string, "AssetBack_"))
			{
				sscanf(posInfoNb + strlen("AssetBack_"), "%d", &actualAsset);
				actualType = AssetBack;
			}
			else if (posInfoNb = strstr(string, "AssetFront_"))
			{
				sscanf(posInfoNb + strlen("AssetFront_"), "%d", &actualAsset);
				actualType = AssetFront;
			}
			else if (strstr(string, "Enemy") != NULL)
			{
				actualType = Enemy;
			}
			else if (strstr(string, "Collider") != NULL)
			{
				actualType = Collider;
			}


			fgets(string, 1500, mapFile);

			// check if file is not corrupt
			if (strstr(string, "<data") == NULL)
			{
				printf("ERRROR DATA CORRUPTED\n");
				fclose(mapFile);
				exit(0);
			}
			//READ DATA
			//height  First to read line by line

			for (int i = 0; i < map.height; i++)
			{
				for (int j = 0; j < map.width; j++)
				{

					switch (actualType)
					{
					case (AssetBack):
						fscanf(mapFile, "%d,", &tmpID);
						map.assetBack[actualAsset][j][i] = CorrectiveIdTile(tmpID, nbTileSet, firstgid);
						break;

					case (AssetFront):
						fscanf(mapFile, "%d,", &tmpID);
						map.assetFront[actualAsset][j][i] = CorrectiveIdTile(tmpID, nbTileSet, firstgid);
						break;

					case (Enemy):
						fscanf(mapFile, "%d,", &tmpID);
						map.enemyTile[j][i] = CorrectiveIdTile(tmpID, nbTileSet, firstgid);

						break;

					case (Collider):
						fscanf(mapFile, "%d,", &tmpID);
						map.colliderID[j][i] = CorrectiveIdTile(tmpID, nbTileSet, firstgid);
						break;

					default: printf("Wrong ActualType.\n");
						break;
					}
				}
			}
		}

		//Enter in Read Data MODE if string contain "<imagelayer"
		if (strstr(string, "<imagelayer") != NULL)
		{
			ImageTileType imageTileType = 0;
			sfVector2f imagePos = { 0 };
			char name[100] = { 0 };
			char nameFile[200] = { 0 };
			float imageLayerF = 0;
			int imageLayer = 0;
			int imageID = 0;

			//read Pos
			sscanf(strstr(string, "offsetx=\"") + strlen("offsetx=\""), "%f", &imagePos.x);
			sscanf(strstr(string, "offsety=\"") + strlen("offsety=\""), "%f", &imagePos.y);

			//setThe type to know who chainlist use
			sscanf(strstr(string, "name=") + strlen("name="), "%s", name);
			imageTileType = strstr(name, "Front") != NULL;

			if (imageTileType == back)
			{
				sscanf(strstr(string, "ImageBack_") + strlen("ImageBack_"), "%f", &imageLayerF);
				imageLayer = 50 + (imageLayerF * 2.f);
				//printf("layer : %d\n", imageLayer);

			}
			else
			{
				sscanf(strstr(string, "ImageFront_") + strlen("ImageFront_"), "%f", &imageLayerF);
				imageLayer = 20 - (imageLayerF * 2.f);
				//printf("layer : %d\n", imageLayer);

			}


			//NextLine to get ID
			fgets(string, 1500, mapFile);

			if (strstr(string, "<image") == NULL)
			{
				printf("FILE CORRUPTED: map file\n");
				fclose(mapFile);
				exit(0);
			}

			//get the name of Image
			sscanf(strstr(string, "source=\"") + strlen("source=\""), "%s", &nameFile);

			//look if there is "/" in the source to get only the map name
			//printf("%s\n", nameFile);

			char* tmpPos = NULL;
			sfBool skip = sfFalse;
			while ((tmpPos = strstr(nameFile, "/")) != NULL && !skip)
			{
				if (tmpPos == strstr(nameFile, "/>"))
				{
					skip = sfTrue;
				}
				else
				{
					sscanf(tmpPos + 1, "%s", nameFile);
				}
			}

			//printf("%s\n", nameFile);


			char* toReplace = strchr(nameFile, '.');
			*toReplace = '\0';


			//Find id
			imageID = GetImageID(nameFile);


			//ADD TO CHAIN LIST
			if (imageTileType == back)
			{
				AddImageTileFront(&map.backImage, imagePos, imageLayer, imageID);
			}
			else
			{
				AddImageTileFront(&map.frontImage, imagePos, imageLayer, imageID);
			}
		}
	}



	//CHECK IF DEBUG PRINTF
#ifdef DEBUG_LOAD_MAP

	for (int nbA = 0; nbA < map.nbAssetBack; nbA++)
	{
		printf("-----AssetBack_%d-----\n", nbA);
		for (int j = 0; j < map.height; j++)
		{
			for (int i = 0; i < map.width; i++)
			{
				printf("%d", map.assetBack[nbA][i][j]);
				if (i != map.width - 1)
				{
					printf(",");
				}
			}
			printf("\n");
		}
	}


	printf("\n-----Enemy-----\n");
	for (int j = 0; j < map.height; j++)
	{
		for (int i = 0; i < map.width; i++)
		{
			printf("%d", map.enemyTile[i][j]);
			if (i != map.width - 1)
			{
				printf(",");
			}
		}
		printf("\n");
	}

	printf("\n-----colliderID-----\n");
	for (int j = 0; j < map.height; j++)
	{
		for (int i = 0; i < map.width; i++)
		{
			printf("%d", map.colliderID[i][j]);
			if (i != map.width - 1)
			{
				printf(",");
			}
		}
		printf("\n");
	}

#endif

	fclose(mapFile);
	return map;
}

// ---------------------------------------------------------------------------------------------
InstanceMaps ReadMap(char area)
{
	InstanceMaps instance;
	MapGenerated result;
	RandBounds randBounds;
	randBounds.max = 0;
	randBounds.min = 0;
	instance.actualMap = 0;

	char mapCount = 0;
	char indexResult = 0;

	char fileName[100];
	sprintf(fileName, "Maps/Area%d/MapsInfos.dat", area + 1);
	FILE* f = fopen(fileName, "rt");

	char buff[50];
	int nbMaps = 0;

	if (f == NULL)
	{
		printf("Erreur lecture fichier maps\n");
		exit(EXIT_FAILURE);
	}

	fscanf(f, "%s", buff);

	fscanf(f, "%d", &randBounds.min);
	fscanf(f, "%d", &randBounds.max);

	nbMaps = GetAreaMapNumber(f);

	rewind(f);

	GenerateMapPattern(&result, randBounds.min, randBounds.max, nbMaps);

	instance.maps = (TiledMap*)malloc(result.mapNb * sizeof(TiledMap));
	instance.mapsNb = result.mapNb;

	qsort(result.mapArray, result.mapNb, sizeof(MapSort), QsortMapInstance);

	//get area

	fscanf(f, "%s", buff);
	fscanf(f, "%d", &randBounds.min);
	fscanf(f, "%d", &randBounds.max);

	do
	{
		fscanf(f, "%s", buff);
		if (strcmp(buff, "name:") == 0)
		{
			if (result.mapArray[indexResult].mapNum == mapCount)
			{
				char mapName[50];
				char actualInstancePos;

				fscanf(f, "%s", &mapName);


				actualInstancePos = result.mapArray[indexResult].instancePos;
				instance.maps[actualInstancePos] = ReadTiled(mapName, area + 1);
				indexResult++;

			}

			mapCount++;
		}

	} while (!feof(f) && indexResult < result.mapNb);

	//free(result.mapArray);
	fclose(f);

	return instance;
}

// ---------------------------------------------------------------------------------------------
void ReadSvg()
{
	FILE* file;
	char fileName[100];
	sprintf(fileName, "Maps/Area1/MapCollisionFile.svg");
	file = fopen(fileName, "rt");
	char buffer[1000] = { 0 };

	//while (fgets(buffer, 1000, file) != NULL)
	//{
	//	printf("%s\n", buffer);
	//}
	fclose(file);
}

// ---------------------------------------------------------------------------------------------









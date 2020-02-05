#include "SpriteBank.h"

void CutSpriteSheet(sfSprite* **sprites, sfTexture* texture, char* filePath, sfVector2u spriteSize,int index)
{
	sfVector2u spriteSheetSize;
	sfIntRect spriteRect;

	int subDivisionX;
	int subDivisionY;

	spriteSheetSize = sfTexture_getSize(texture);

	subDivisionX = spriteSheetSize.x / spriteSize.x;
	subDivisionY = spriteSheetSize.y / spriteSize.y;

	spriteRect.height = spriteSize.x;
	spriteRect.width = spriteSize.y;

	sprites[index] = (sfSprite**)malloc((subDivisionX * subDivisionY) * sizeof(sfSprite*));

	for (int y = 0; y < subDivisionY; y++)
	{
		spriteRect.top = spriteRect.height * y;
		for (int x = 0; x < subDivisionX; x++)
		{
			int spriteIndex;

			spriteIndex = x + y * subDivisionX;
			spriteRect.left = spriteRect.width * x;
			
			//sfTexture* tmpTexture = sfTexture_createFromFile(filePath, &spriteRect);
			sprites[index][spriteIndex] = sfSprite_create();
			sfTexture_setSmooth(texture, sfFalse);
			sfSprite_setTexture(sprites[index][spriteIndex], texture, sfTrue);
			sfSprite_setTextureRect(sprites[index][spriteIndex], spriteRect);
		}
	}
}

MapSprites LoadMapSprites()
{
	MapSprites tmpMapSprites;
	sfVector2u blockSize = { SIZE_TILE, SIZE_TILE };

	tmpMapSprites.Tileset_Back = (sfSprite***)malloc( 3 * sizeof(sfSprite**));
	for (int i = 0; i < 1; i++)
	{
		char filePath[200];
		sprintf(filePath, "Maps/Area%d/TileSet_Back.png", i+1);
		sfTexture* tmpTexture = sfTexture_createFromFile(filePath, NULL);
		CutSpriteSheet(tmpMapSprites.Tileset_Back, tmpTexture, filePath, blockSize,i);
	}

	tmpMapSprites.Tileset_Bloc = (sfSprite***)malloc(3 * sizeof(sfSprite**));
	for (int i = 0; i < 1; i++)
	{
		char filePath[200];
		sprintf(filePath, "Maps/Area%d/Tileset_Bloc.png", i + 1);
		sfTexture* tmpTexture = sfTexture_createFromFile(filePath, NULL);
		CutSpriteSheet(tmpMapSprites.Tileset_Bloc, tmpTexture, filePath, blockSize, i);
	}

	tmpMapSprites.Tileset_Front = (sfSprite***)malloc(3 * sizeof(sfSprite**));
	for (int i = 0; i < 1; i++)
	{
		char filePath[200];
		sprintf(filePath, "Maps/Area%d/TileSet_Front.png", i + 1);
		sfTexture* tmpTexture = sfTexture_createFromFile(filePath, NULL);
		CutSpriteSheet(tmpMapSprites.Tileset_Front, tmpTexture, filePath, blockSize, i);
	}

	return tmpMapSprites;
}

RotatingBackpackSprite LoadRotatingBPSprite()
{
	RotatingBackpackSprite temp;
	temp.engrenages[0] = LoadSprite("Assets/Hud/engrenages/Emplacement_Vert_1.png", 1);
	temp.engrenages[1] = LoadSprite("Assets/Hud/engrenages/Emplacement_Vert_2.png", 1);
	temp.engrenages[2] = LoadSprite("Assets/Hud/engrenages/Emplacement_Vert_3.png", 1);
	temp.engrenages[3] = LoadSprite("Assets/Hud/engrenages/Emplacement_Bleu_4.png", 1);
	temp.engrenages[4] = LoadSprite("Assets/Hud/engrenages/Emplacement_Bleu_5.png", 1);
	temp.engrenages[5] = LoadSprite("Assets/Hud/engrenages/Emplacement_Rouge_6.png", 1);
	temp.engrenages[6] = LoadSprite("Assets/Hud/engrenages/Emplacement_Rouge_7.png", 1);
	temp.engrenages[7] = LoadSprite("Assets/Hud/engrenages/Emplacement_Rouge_8.png", 1);

	temp.grosEngrenage = LoadSprite("Assets/Hud/GrandEngrenage.png", 1);
	return temp;
}

BackpackSprite LoadBPSprite()
{
	BackpackSprite temp;
	temp.selecteur = LoadSprite("Assets/Hud/Curseur.png", 1);
	temp.items[0] = LoadSprite("Assets/Item/offensifs/Dgts.png", 0);
	temp.items[1] = LoadSprite("Assets/Item/offensifs/DgtsElem.png", 0);
	temp.items[2] = LoadSprite("Assets/Item/offensifs/DgtsSkills.png", 0);
	temp.items[3] = LoadSprite("Assets/Item/offensifs/ChanceCrit.png", 0);
	temp.items[4] = LoadSprite("Assets/Item/offensifs/DgtsCrit.png", 0);
	temp.items[5] = LoadSprite("Assets/Item/offensifs/CDR.png", 0);

	temp.items[6] = LoadSprite("Assets/Item/defensifs/armure.png", 0);
	temp.items[7] = LoadSprite("Assets/Item/defensifs/hp.png", 0);
	temp.items[8] = LoadSprite("Assets/Item/defensifs/RegenHp.png", 0);
	temp.items[9] = LoadSprite("Assets/Item/defensifs/Stamina.png", 0);
	temp.items[10] = LoadSprite("Assets/Item/defensifs/RegenStamina.png", 0);
	temp.items[11] = LoadSprite("Assets/Item/defensifs/ReducCoutStamina.png", 0);

	temp.items[12] = LoadSprite("Assets/Item/skills/Dash.png", 0);
	temp.items[13] = LoadSprite("Assets/Item/skills/Jump.png", 0);
	temp.items[14] = LoadSprite("Assets/Item/skills/ShockWaveEpee.png", 0);

	temp.items[15] = LoadSprite("Assets/Item/craft/CorneCervours.png", 0);
	temp.items[16] = LoadSprite("Assets/Item/craft/NoisetteLoot.png", 0);
	temp.items[17] = LoadSprite("Assets/Item/craft/QueueLoulpe.png", 0);


	temp.dents = LoadSprite("dents.png", 0);
	temp.engrenage = LoadSprite("Engrenage_Inventaire.png", 1);
	temp.grille = LoadSprite("grille.png", 0);
	temp.fondGrille = LoadSprite("fond_grille.png", 0);
	return temp;
}

ItemSprites InitItemSprites()
{
	ItemSprites tmp;
	tmp.items = (sfSprite**)malloc(15 * sizeof(sfSprite*));

	tmp.items[0] = LoadSprite("Assets/Item/offensifs/Dgts.png", 0);
	tmp.items[1] = LoadSprite("Assets/Item/offensifs/DgtsElem.png", 0);
	tmp.items[2] = LoadSprite("Assets/Item/offensifs/DgtsSkills.png", 0);
	tmp.items[3] = LoadSprite("Assets/Item/offensifs/ChanceCrit.png", 0);
	tmp.items[4] = LoadSprite("Assets/Item/offensifs/DgtsCrit.png", 0);
	tmp.items[5] = LoadSprite("Assets/Item/offensifs/CDR.png", 0);

	tmp.items[6] = LoadSprite("Assets/Item/defensifs/armure.png", 0);
	tmp.items[7] = LoadSprite("Assets/Item/defensifs/hp.png", 0);
	tmp.items[8] = LoadSprite("Assets/Item/defensifs/RegenHp.png", 0);
	tmp.items[9] = LoadSprite("Assets/Item/defensifs/Stamina.png", 0);
	tmp.items[10] = LoadSprite("Assets/Item/defensifs/RegenStamina.png", 0);
	tmp.items[11] = LoadSprite("Assets/Item/defensifs/ReducCoutStamina.png", 0);

	tmp.items[12] = LoadSprite("Assets/Item/skills/Dash.png", 0);
	tmp.items[13] = LoadSprite("Assets/Item/skills/Jump.png", 0);
	tmp.items[14] = LoadSprite("Assets/Item/skills/ShockWaveEpee.png", 0);

	return tmp;
}

TileImage LoadTileSprite()
{
	TileImage tileImage;

	char string[500] = { 0 };
	char* posStr = NULL;
	char tmpName[300] = { 0 };
	FILE* dataFile = fopen("Assets/TileImage/tileImage.dat", "r");
	int actualIndex = 0;

	tileImage.nbSprite = 0;

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
			tileImage.nbSprite++;
		}
	}

	tileImage.sprite = malloc(tileImage.nbSprite * sizeof(sfSprite*));

	rewind(dataFile);

	while (!feof(dataFile))
	{
		fgets(string, 500, dataFile);

		if (strstr(string, "<Asset") != NULL)
		{
			posStr = strstr(string, "name=");
			sscanf(posStr + strlen("name="), "%s", tmpName);
			char imageFileName[200] = { 0 };
			sprintf(imageFileName, "Assets/TileImage/%s.png", tmpName);
			tileImage.sprite[actualIndex] = LoadSprite(imageFileName, sfFalse);

			actualIndex++;
		}
	}

	fclose(dataFile);
	return tileImage;
}
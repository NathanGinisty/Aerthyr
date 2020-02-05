#include "animManager.h"


AnimSprite LoadAnimation(char* sNom, int origin, int nbImage)
{
	AnimSprite tmp;

	tmp.sprite = malloc(nbImage * sizeof(sfSprite*));

	for (int i = 0; i < nbImage; i++)
	{
		sfTexture* tempTex;
		char file[255];
		char num[255];
		strcpy(file, "Assets/");
		strcat(file, sNom);
		sprintf(num, "%d", i + 1);
		strcat(file, num);
		strcat(file, ".png");

		//printf("file : %s\n", file);

		tempTex = sfTexture_createFromFile(file, NULL);

		tmp.sprite[i] = sfSprite_create();
		sfSprite_setTexture(tmp.sprite[i], tempTex, 0);

		if (origin == 0)
		{
			sfVector2u tailleImage = sfTexture_getSize(tempTex);
			sfVector2f centre = { (float)tailleImage.x, (float)tailleImage.y };
			sfSprite_setOrigin(tmp.sprite[i], (sfVector2f) { centre.x, centre.y });
		}
		else if (origin == 1)
		{
			sfVector2u tailleImage = sfTexture_getSize(tempTex);
			sfVector2f centre = { (float)tailleImage.x / 2, (float)tailleImage.y / 2 };
			sfSprite_setOrigin(tmp.sprite[i], (sfVector2f) { centre.x, centre.y });
		}
		else if (origin == 2)
		{
			sfVector2u tailleImage = sfTexture_getSize(tempTex);
			sfVector2f centre = { (float)tailleImage.x / 2, (float)tailleImage.y / 2 };
			sfSprite_setOrigin(tmp.sprite[i], (sfVector2f) { centre.x, centre.y * 2 });
		}
	}
	return tmp;
}

AnimSprite* LoadAnimSpriteByFile(char* filename)
{
	AnimSprite* tmp;

	char fileadress[200];
	strcpy(fileadress, "Data/");
	strcat(fileadress, filename);
	strcat(fileadress, ".txt");

	FILE* f = fopen(fileadress, "r");
	if (f == NULL)
	{
		printf("Erreur d'ouverture de fichier ! : animSprite \n");
		exit(EXIT_FAILURE);
	}

	char tempNom[255];
	int state = 0;
	int nbAnim = 0;
	char adressAnim[200];
	float spd = 0.f;
	int origin = 0;

	// Nombre d'anim différente
	while (fscanf(f, "%s", tempNom) != EOF)
	{
		if (strcmp(tempNom, "ANIM") == 0)
		{
			nbAnim++;
		}
		//printf("%s\n", tempNom);
	}
	//printf("Nbanim = %d\n", nbAnim);
	tmp = (AnimSprite*) malloc(nbAnim * sizeof(AnimSprite));

	rewind(f);

	// Load des anims une par une
	while (fscanf(f, "%s", tempNom) != EOF)
	{
		if (strcmp(tempNom, "ANIM") == NULL)
		{
			// Nom de l'anim
			fscanf(f, "%d", &state); //state
		}
		// Paramètre de l'anim
		if (strcmp(tempNom, "nb") == NULL)
		{
			fscanf(f, "%d", &nbAnim);
		}
		if (strcmp(tempNom, "adr") == NULL)
		{
			fscanf(f, "%s", adressAnim);
		}
		if (strcmp(tempNom, "origin") == NULL)
		{
			fscanf(f, "%d", &origin);
			tmp[state] = LoadAnimation(adressAnim, origin, nbAnim);
		}
	}

	fclose(f);

	return tmp;
}


AnimInfo LoadAnimInfo(float speed, int nbImage)
{
	AnimInfo tmp;
	tmp.nbImage = nbImage;
	tmp.speedAnim = speed;
	tmp.actualImage = 0;
	tmp.timer = 0;

	return tmp;
}

AnimInfo* LoadAnimInfoByFile(char* filename)
{
	AnimInfo* tmp;

	char fileadress[200];
	strcpy(fileadress, "Data/");
	strcat(fileadress, filename);
	strcat(fileadress, ".txt");

	FILE* f = fopen(fileadress, "r");
	if (f == NULL)
	{
		printf("Erreur d'ouverture de fichier ! : anim info\n");
		exit(EXIT_FAILURE);
	}

	char tempNom[255];
	int state = 0;
	int nbAnim = 0;
	char adressAnim[100];
	float spd = 0.f;
	int centered = 0;

	// Nombre d'anim différente
	while (fscanf(f, "%s", tempNom) != EOF)
	{
		if (strcmp(tempNom, "ANIM") == 0)
		{
			nbAnim++;
		}
	}
	tmp = (AnimInfo*) malloc(nbAnim * sizeof(AnimInfo));

	rewind(f);

	// Load des anims une par une
	while (fscanf(f, "%s", tempNom) != EOF)
	{
		if (strcmp(tempNom, "ANIM") == 0)
		{
			// Nom de l'anim by téking the state
			fscanf(f, "%d", &state);
		}
		if (strcmp(tempNom, "nb") == 0)
		{
			fscanf(f, "%d", &nbAnim);
			tmp[state].nbImage = nbAnim;
		}
		if (strcmp(tempNom, "spd") == 0)
		{
			fscanf(f, "%f", &spd);
			tmp[state].speedAnim = spd;
		}
		tmp[state] = LoadAnimInfo(tmp[state].speedAnim, tmp[state].nbImage);
	}

	fclose(f);


	return tmp;
}


void UpdateAnimation(AnimInfo *animInfo, AnimSprite *animSprite, float dt)
{
	animInfo->timer += 1 * dt;

	if (animInfo->timer >= animInfo->speedAnim)
	{
		if (animInfo->actualImage < animInfo->nbImage)
		{
			animInfo->actualImage++;
		}
		if (animInfo->actualImage >= animInfo->nbImage)
		{
			animInfo->actualImage = 0;
		}
		animInfo->timer = 0;
	}

}


void RenderAnimation(AnimInfo animInfo, AnimSprite animSprite, sfVector2f pos, sfRenderWindow* window)
{
	sfSprite_setPosition(animSprite.sprite[animInfo.actualImage], pos);
	sfRenderWindow_drawSprite(window, animSprite.sprite[animInfo.actualImage], NULL);
}


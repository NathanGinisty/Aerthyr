#include "Parallax.h"

#define NBLAYERPARALLAX 3
#define BGLAYER_MIN 70

#define SPRITECLOUDW 800

ParallaxObjectPack InitParallaxPack()
{
	ParallaxObjectPack tmp;
	tmp.firstParticle = (ParallaxObject*)calloc(sizeof(ParallaxObject), 1);
	tmp.wind = rand() % 50 + 100;
	tmp.nbObject = 0;
	return tmp;
}

ParallaxSprite LoadParallaxSprite()
{
	ParallaxSprite tmpSprites;

	char filePath[100];

	sprintf(filePath, "Assets/Parallax/background_%d.png", 1);
	tmpSprites.background[0] = LoadSprite(filePath, sfFalse);

	// Cloud
	tmpSprites.cloud = malloc(sizeof(sfSprite*) * 4);
	for (int nbImg = 0; nbImg < 4; nbImg++)
	{
		sprintf(filePath, "Assets/Parallax/Clouds/cloud_%d.png", nbImg + 1);
		//tmpSprites.cloud[nbImg] = LoadSprite(filePath, sfTrue);

		sfTexture* tempTex;
		sfSprite* tempSprite;
		tempTex = sfTexture_createFromFile(filePath, NULL);
		sfTexture_setSmooth(tempTex, sfTrue);

		tempSprite = sfSprite_create();
		sfSprite_setTexture(tempSprite, tempTex, 0);

		sfVector2u tailleImage = sfTexture_getSize(tempTex);
		sfVector2f centre = { tailleImage.x / 2.f, tailleImage.y / 2.f };
		sfSprite_setOrigin(tempSprite, centre);

		tmpSprites.cloud[nbImg] = tempSprite;
	}

	// Isle
	tmpSprites.isle = malloc(sizeof(sfSprite*) * 7);

	for (int nbImg = 0; nbImg < 7; nbImg++)
	{
		tmpSprites.isle[nbImg] = malloc(sizeof(sfSprite*) * 2);
		for (int typeSprite = 0; typeSprite < 2; typeSprite++)
		{
			sprintf(filePath, "Assets/Parallax/Isles/isle_%d_%d.png", nbImg + 1, typeSprite + 1);
			tmpSprites.isle[nbImg][typeSprite] = LoadSprite(filePath, sfTrue);
		}
	}
	return tmpSprites;
}

void CreateParallax(ParallaxObjectPack* p, sfVector2f minPos, sfVector2f maxPos)
{
	int mapH = (int)(maxPos.y / SIZE_TILE);
	int mapW = (int)(maxPos.x / SIZE_TILE);
	int nbSpawn = (int)(mapH * mapW / 15);
	printf("nbSpawn : %d", nbSpawn);
	for (int i = 0; i < nbSpawn; i++)
	{
		p->nbObject++;

		ParallaxObject* tmp = (ParallaxObject*)calloc(sizeof(ParallaxObject), 1);
		int randPosX = rand() % (int)maxPos.x + (int)minPos.x;
		int randPosY = rand() % (int)maxPos.y + (int)minPos.y;
		tmp->pos = (sfVector2f) { (float)randPosX, (float)randPosY };
		tmp->timer = (float)(rand() % 1000);
		tmp->angle = rand() % 360; //(float)(rand() % 360);

		tmp->type = rand() % 3;

		if (tmp->type == cloud)
		{
			tmp->spd = (float)(rand() % (int)p->wind - 1 - (p->wind * 2));
			tmp->sprUsed = rand() % 4;

			int inBackground = rand() % 100;
			if (inBackground < 90)
			{
				tmp->layer = BGLAYER_MIN + rand() % NBLAYERPARALLAX;
				float tempScale = (float)(rand() % 100 + 300) / 100;
				tmp->scale = (sfVector2f) { tempScale, tempScale };
				tmp->color = (sfColor) { 255, 255, 255, 160 };
			}
			else
			{
				tmp->layer = 4;
				float tempScale = (float)(rand() % 400 + 600) / 100;
				tmp->scale = (sfVector2f) { tempScale, tempScale };
				tmp->color = (sfColor) { 255, 255, 255, 70 };
			}
		}
		else if (tmp->type == isle)
		{
			tmp->spd = (float)(rand() % 2 * (NBLAYERPARALLAX - tmp->layer) + 5 * (NBLAYERPARALLAX - tmp->layer));
			tmp->sprUsed = rand() % 7;
			tmp->layer = BGLAYER_MIN + rand() % NBLAYERPARALLAX;

			switch (tmp->layer)
			{
			case (BGLAYER_MIN + 0):
				tmp->spd = (float)(rand() % 2 * (NBLAYERPARALLAX - 0) + 5 * (NBLAYERPARALLAX - 0));
				tmp->scale = (sfVector2f) { 1 * (rand() % 2 * 2 - 1), 1 };
				tmp->color = sfColor_fromRGBA(255, 255, 255, 60);
				break;
			case (BGLAYER_MIN + 1):
				tmp->spd = (float)(rand() % 2 * (NBLAYERPARALLAX - 1) + 5 * (NBLAYERPARALLAX - 1));
				tmp->scale = (sfVector2f) { 0.5* (rand() % 2 * 2 - 1), 0.5 };
				tmp->color = sfColor_fromRGBA(255, 255, 255, 130);
				break;
			case (BGLAYER_MIN + 2):
				tmp->spd = (float)(rand() % 2 * (NBLAYERPARALLAX - 2) + 5 * (NBLAYERPARALLAX - 2));
				tmp->scale = (sfVector2f) { 0.2* (rand() % 2 * 2 - 1), 0.2 };
				tmp->color = sfColor_fromRGBA(255, 255, 255, 220);
				break;
			}
		}

		//Insertion de l'élément
		if (p->nbObject > 0)
		{
			tmp->next = p->firstParticle;
			p->firstParticle = tmp;
		}
		else
		{
			p->firstParticle = tmp;
		}
	}
}

void DeleteParallaxObject(ParallaxObject* current, ParallaxObjectPack* pack)
{
	struct ParallaxObject* tmp = pack->firstParticle;

	if (tmp == current) //Cas particulier du premier élément
	{
		pack->firstParticle = current->next;
		free(current);
		pack->nbObject--;
	}
	else //Si ce n'est pas le premier élément
	{
		while (tmp != NULL)
		{
			if (tmp->next == current)
			{
				tmp->next = tmp->next->next;
				free(current);
				pack->nbObject--;
			}
			tmp = tmp->next;
		} 
	}
}

void DeleteAllParallaxObject(ParallaxObjectPack* pack)
{
	if (pack->firstParticle != NULL)
	{
		struct ParallaxObject* tmp = pack->firstParticle;
		struct ParallaxObject* saved = tmp->next;

		while (tmp != NULL)
		{
			if (tmp != NULL)
			{
				DeleteParallaxObject(tmp, pack);
				tmp = saved;
			}

			if (tmp != NULL)
			{
				saved = tmp;
				tmp = tmp->next;
			}
		}
		pack->nbObject = 0;
	}
}


void UpdateParallax(ParallaxObjectPack *p, CameraViews *camera, Player player, sfVector2i mapSize, float dt)
{
	// ----- NB OBJECT
	p->nbObject = 0;
	for (ParallaxObject* tmp = p->firstParticle; tmp != NULL; tmp = tmp->next)
	{
		p->nbObject++;
	}

	// ------ Update pos
	static float timer = 0;
	static int PrecedentDir = 0;
	static float decalCam = 0;

	sfVector2f diff = GetDiffCameraPlayer(*camera, player);

	// timer decalCam
	if (player.entity2D.dir == PrecedentDir && player.entity2D.body.vel.x != 0)
	{
		timer += dt;
	}
	else
	{
		timer = 0;
		decalCam = 0;
	}

	// reset decalCam
	//if (player.entity2D.dir != PrecedentDir)
	//{
	//	decalCam = 0;
	//}

	// precedentDir
	PrecedentDir = player.entity2D.dir;

	//timer
	if (timer > 0.1)
	{
		if (decalCam + camera->size.x / 2 < camera->size.x / 3 * 2 &&
			decalCam + camera->size.x / 2 > camera->size.x / 3 * 1)
		{
			decalCam += 200 * dt;
		}
	}

	diff.x += (decalCam * camera->scale * PrecedentDir);

	// if (bord de la map), pas de mouvement
	if (camera->pos.x <= 0)
	{
		diff.x = 0;
	}
	if (camera->pos.x >= mapSize.x * SIZE_TILE)
	{
		diff.x = 0;
	}
	if (camera->pos.y <= 0)
	{
		diff.y = 0;
	}
	if (camera->pos.y >= mapSize.y * SIZE_TILE)
	{
		diff.y = 0;
	}


	// t
	float t = 0.4 / (player.stats.movespd / 1000);

	for (ParallaxObject* tmp = p->firstParticle; tmp != NULL; tmp = tmp->next)
	{
		//V=D/t
		float vX = diff.x / (t * (1.0 + (float)((NBLAYERPARALLAX - (tmp->layer - BGLAYER_MIN) * 1.2))));
		float vY = diff.y / (t * (0.5 + (float)((NBLAYERPARALLAX - (tmp->layer - BGLAYER_MIN) * 0.6))));

		//D = v*t
		tmp->pos.x += vX * dt;
		tmp->pos.y += vY * dt;

		// isle
		if (tmp->type == isle)
		{
			tmp->timer += dt;
			tmp->pos.y += (tmp->spd * sin(tmp->timer)) * dt;
		}

		// cloud
		else if (tmp->type == cloud)
		{
			//pushed by da wind
			tmp->pos.x += (p->wind + tmp->spd) * dt;

			//respawn if map limit

			if (tmp->pos.x >= (mapSize.x + PARALLAXDECAL)* SIZE_TILE + SPRITECLOUDW)
			{
				tmp->pos.x = -PARALLAXDECAL * SIZE_TILE - SPRITECLOUDW;
				tmp->pos.y = rand() % mapSize.y * SIZE_TILE;
			}
			else if (tmp->pos.x <= -PARALLAXDECAL * SIZE_TILE - SPRITECLOUDW)
			{
				tmp->pos.x = (mapSize.x + PARALLAXDECAL)* SIZE_TILE + SPRITECLOUDW;
				tmp->pos.y = rand() % mapSize.y * SIZE_TILE;
			}
		}


	}
}

void RenderBackground(sfRenderWindow* window, SpritePack sprite)
{
	BlitSprite(sprite.parallax.background[0], (sfVector2f) { 0, 0 }, window);
}

void RenderParallax(sfRenderWindow* window, ParallaxObjectPack *p, CameraViews cam, SpritePack sprite, Layers *layer)
{
	InfoBlitSprite tmpInfoObject = DEFAULT_INFOBLIT;

	for (ParallaxObject* tmp = p->firstParticle; tmp != NULL; tmp = tmp->next)
	{
		sfBool topColl = tmp->pos.y > cam.pos.y - cam.size.y * 2;
		sfBool botColl = tmp->pos.y < cam.pos.y + cam.size.y * 2;
		sfBool leftColl = tmp->pos.x > cam.pos.x - cam.size.x * 2;
		sfBool rightColl = tmp->pos.x < cam.pos.x + cam.size.x * 2;

		tmpInfoObject.active = sfTrue;
		tmpInfoObject.scale = tmp->scale;

		if (topColl && botColl && leftColl && rightColl)
		{
			if (tmp->type == isle)
			{

				InfoBlitSprite tmpInfoLight = DEFAULT_INFOBLIT;
				tmpInfoLight.active = sfTrue;
				tmpInfoLight.color = tmp->color;
				tmpInfoLight.scale = tmp->scale;

				addToLayer(&layer[tmp->layer].premier, &sprite.parallax.isle[tmp->sprUsed][Prx_light], tmp->pos, tmpInfoLight);

				addToLayer(&layer[tmp->layer].premier, &sprite.parallax.isle[tmp->sprUsed][Prx_object], tmp->pos, tmpInfoObject);
			}

			else if (tmp->type == cloud)
			{
				InfoBlitSprite tmpInfoCloud = DEFAULT_INFOBLIT;
				tmpInfoCloud.active = sfTrue;
				tmpInfoCloud.color = tmp->color;
				tmpInfoCloud.scale = tmp->scale;

				//layer 17-14
				addToLayer(&layer[tmp->layer].premier, &sprite.parallax.cloud[tmp->sprUsed], tmp->pos, tmpInfoCloud);
			}
		}
	}
}

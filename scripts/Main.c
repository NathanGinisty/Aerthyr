/*
******* ERREUR / PROBLEME ********



******* A FAIRE ********



*/
// binds used: T G Y Espace D Q ECHAP + - P num9 num8 O I

#include "LoaderManager\loaderManager.h"


//---------------------------------------------------------------------------------------------------------------------


/// Main function which call all others
int _tmain(int argc, _TCHAR* argv[])
{
	sfEvent event;
	sfClock* dtClock = sfClock_create();
	srand((unsigned int)time(NULL));

	float deltaTime = 0.0f;
	float gameTime = 0.0f;

	GameData gameData;



	sfVector2i mousePerma;
	sfVector2f posSelect = { 0,0 };
	TextInfo text;
	CreateText(&text, "Assets/Hud/Arial.ttf");

	sfSprite* menuSprite = LoadSprite("baleine.png", 0);

	void(**pGameState)(sfVector2i mouse, Fade* fade, sfRenderWindow* window) = (void*)malloc(4 * (sizeof(sfVector2i) + sizeof(Fade*) + sizeof(sfRenderWindow*)));
	pGameState[menu] = &Menu;
	pGameState[jeu] = &Jeu;
	pGameState[options] = &Options;
	pGameState[credits] = &Credits;

	sfVector2i curseurMenu = { 1650, 500 };

	//init gamedata
	{
		sfBool endThread = sfFalse;
		DataThread1 data;
		data.gameData = &gameData;
		data.endThead = &endThread;
		//creation of load thread
		sfThread* threadGameData = sfThread_create(&ChargementGameDataThread, &data);
		sfThread_launch(threadGameData);
		LoaderDisplay(&data, deltaTime, dtClock);
		sfThread_destroy(threadGameData);
		//end of loading game launch 
	}

#pragma region loadOptions
	FILE* f_options = fopen("Launcher/InitParameters.txt", "r");
	if (f_options == NULL)
	{
		gameData.option = DEFAULTOPTIONS;
	}
	else
	{
		while (!feof(f_options))
		{
			char buff[200] = { 0 };
			fgets(buff, 200, f_options);
			if (strstr(buff, "<reso") != NULL)
			{
				sscanf(strstr(buff, "<reso") + strlen("<reso "), "%d", &gameData.option.resolution.x);
				sscanf(strstr(buff, "<reso") + strlen("<reso ") + 4, "%d", &gameData.option.resolution.y);
			}
			if (strstr(buff, "<fen") != NULL)
			{
				sscanf(strstr(buff, "<fen") + strlen("<fen "), "%d", &gameData.option.windowStyle);
				switch (gameData.option.windowStyle)
				{
				case 0: gameData.option.windowStyle = Window;
					break;
				case 1: gameData.option.windowStyle = Borderless;
					break;
				case 2: gameData.option.windowStyle = Fullscreen;
					break;
				default: gameData.option.windowStyle = Window;
					break;
				}
			}

			if (strstr(buff, "<0") != NULL)
			{
				sscanf(strstr(buff, "<0") + strlen("<0 "), "%d", &gameData.option.volume.master);
			}
			if (strstr(buff, "<1") != NULL)
			{
				sscanf(strstr(buff, "<1") + strlen("<1 "), "%d", &gameData.option.volume.music);
			}
			if (strstr(buff, "<2") != NULL)
			{
				sscanf(strstr(buff, "<2") + strlen("<2 "), "%d", &gameData.option.volume.sounds);
			}


		}
	}
#pragma endregion

	//Ouverture de la fenêtre

	sfVideoMode mode = { gameData.option.resolution.x, gameData.option.resolution.y, 32 };
	sfRenderWindow* window = sfRenderWindow_create(mode, "", gameData.option.windowStyle, NULL);
	//sfRenderWindow* window = sfRenderWindow_create(mode, "", sfFullscreen, NULL);

	gameData.spritePack.shadow = LoadSprite("Assets/Shadow/shadow.png", sfTrue);

	sfSprite* tmpFond = LoadSprite("Assets/Hud/Fond.png", 0);

	sfSprite* flecheDir = LoadSprite("Assets/Debug/fleche.png", 0);
	sfSprite_setOrigin(flecheDir, (sfVector2f) { 0, 20 });

	int* indexMap = &gameData.instance.actualMap;

	//VertexArray Test colli
	sfVertexArray* vertexArr = sfVertexArray_create();
	sfVertexArray_resize(vertexArr, 2);
	sfVertexArray_setPrimitiveType(vertexArr, sfLines);
	sfVertex vertexPoint;

	sfCircleShape* legsCircle = sfCircleShape_create();
	sfCircleShape_setRadius(legsCircle, 5.f);
	sfCircleShape_setOrigin(legsCircle, (sfVector2f) { 5.f, 5.f });
	sfCircleShape_setFillColor(legsCircle, sfBlack);

	sfCircleShape* headCircle = sfCircleShape_create();
	sfCircleShape_setRadius(headCircle, gameData.player.entity2D.bodyCollider.topC.radius);
	sfCircleShape_setOrigin(headCircle, (sfVector2f) { 242.0f / 4, 242.0f / 4 });
	sfCircleShape_setFillColor(headCircle, sfRed);

	sfRectangleShape* rectPlayer;
	sfVector2f rectPos = gameData.player.entity2D.body.pos;
	rectPos.y -= 242.0f / 2;
	rectPlayer = sfRectangleShape_create();
	sfRectangleShape_setSize(rectPlayer, (sfVector2f) { gameData.player.entity2D.bodyCollider.rect.width, gameData.player.entity2D.bodyCollider.rect.height });
	sfRectangleShape_setPosition(rectPlayer, rectPos);
	sfRectangleShape_setOrigin(rectPlayer, (sfVector2f) { 242.0f / 4, 242.0f / 4 });
	sfRectangleShape_setFillColor(rectPlayer, sfGreen);

	// --- Load

	/*sfVector2f parallaxMaxPos = (sfVector2f) { (gameData.instance.maps[0].width + 10) * SIZE_TILE, (gameData.instance.maps[0].height + 10) * SIZE_TILE };
	CreateParallax(&gameData.parallax, (sfVector2f) { -SIZE_TILE * 10, -SIZE_TILE * 10 }, (sfVector2f) { parallaxMaxPos.x, parallaxMaxPos.y });*/

	// --------------------------------- player
	void(**pFunction)(Player*) = (void*)malloc(nbEnumPlayerState * sizeof(Player*));
	pFunction[pla_idle] = &Idle;
	pFunction[pla_walking] = &Walking;
	pFunction[pla_crouch] = &Crouch;
	pFunction[pla_leaningOnAWall] = &LeaningOnAWall;
	pFunction[pla_climbing] = &Climbing;
	pFunction[pla_impulsion] = &Impulsion;
	pFunction[pla_rising] = &Rising;
	pFunction[pla_extremum] = &Extremum;
	pFunction[pla_falling] = &Falling;
	pFunction[pla_gliding] = &Gliding;
	pFunction[pla_landing] = &Landing;
	pFunction[pla_rolling] = &Rolling;
	pFunction[pla_stunned] = &Stunned;
	pFunction[pla_attack] = &Attack;

	// --------------------------------- spell
	void(**spellFunction)(Spell*, StatPlayer*, Entity2D*, Projectile**, float) =
		(void*)malloc(nbEnumSpellName * (sizeof(Spell*) + sizeof(StatPlayer*) + sizeof(Entity2D*) + sizeof(Projectile**) + sizeof(float)));

	spellFunction[spl_minorHeal] = &MinorHeal;
	spellFunction[spl_dotHeal] = &DotHeal;
	spellFunction[spl_tripleJump] = &TripleJump;
	spellFunction[spl_fireBall] = &FireBall;


	sfView_setSize(gameData.camera.view, gameData.camera.size);

	//LoadTiledEnemy(gameData.instance.maps[*indexMap], 1, &gameData.enemyList, &gameData.player.entity2D.middlePos, gameData.enemiesAnimsInfos);

	//Main loop
	deltaTime = sfTime_asSeconds(sfClock_restart(dtClock));


#pragma region LOGO CREA

	sfSprite* logoCrea = LoadSprite("Assets/Logo/Creajeux.png", 0);
	float timerLogo = 0;
#pragma endregion

	sfRenderWindow_setFramerateLimit(window, 250);
	//sfRenderWindow_setVerticalSyncEnabled(window, sfTrue);

	//---------------------------------------- == GAME UPDATE == -----------------------------------------//

	//----------------------------------------------------------------------------------------------------\\

	while (sfRenderWindow_isOpen(window))
	{
		// Initialization of the deltaTime ( for the frame rate ).
		deltaTime = sfTime_asSeconds(sfClock_restart(dtClock));
		gameTime += deltaTime;
		while (sfWindow_pollEvent((sfWindow*)window, &event))
		{
			UpdateCommand(&gameData.command, window, event);

			//si pas dans menu et pas en jeu: B retourne au menu
			if (gameData.gameState != jeu)
			{
				if (gameData.command.comp1Nback.pressed)
				{
					gameData.fade.infos.changeTo = menu;
					gameData.fade.infos.state = F_fadeIn;
				}
			}
			//escape ramene toujours au menu
			if (event.type == sfEvtKeyPressed)
			{
				if (event.key.code == sfKeyEscape)
				{
					gameData.fade.infos.changeTo = menu;
					gameData.fade.infos.state = F_fadeIn;
				}
			}

			if (gameData.gameState == jeu)
			{
				if (event.type == sfEvtClosed)  exit(0);


				if (event.type == sfEvtKeyPressed)
				{
					if (event.key.code == sfKeyR)
					{
						gameData.player = InitPlayer();
					}
					if (event.key.code == sfKeyP)
					{
						sfVector2i mouse = sfMouse_getPositionRenderWindow(window);
						gameData.player.entity2D.body.pos = (sfVector2f) { mouse.x + gameData.camera.pos.x - mode.width / 2, mouse.y + gameData.camera.pos.y - mode.height / 2 };
					}
					if (event.key.code == sfKeyW)
					{
						Spawn_Loot(&gameData.loot, gameData.player.entity2D.body.pos);
					}
					if (event.key.code == sfKeyF)
					{
						Catch_Loot(&gameData.loot, gameData.player.entity2D.middlePos, &gameData.inventaire.backPack);
					}
					//hud keys
					if (event.key.code == sfKeyI)
					{
						AddBackPackItem2(rand() % 3 + 15, rand() % 20 + 1, loot_consumable, &gameData.inventaire.backPack);
					}
					if (!gameData.inventaire.current.isMoving)
					{
						if (event.key.code == sfKeyE)
						{
							gameData.rotatingBP.etat = BP_selectSlot;
						}
						if (event.key.code == sfKeyA)
						{
							gameData.rotatingBP.etat = BP_gestion;
						}
					}
					if (event.key.code == sfKeyO)
					{

						if (gameData.inventaire.current.isMoving == sfFalse)
						{
							if (gameData.inventaire.isOpen)
							{
								gameData.inventaire.isOpen = sfFalse;
								if (gameData.rotatingBP.bagState == BP_opening)
								{
									for (int i = 0; i < QTITY; i++)
									{
										gameData.rotatingBP.engrenages[i].time = INVENTORYTIME - gameData.rotatingBP.engrenages[i].time;
									}
								}
								gameData.rotatingBP.bagState = BP_closing;
								if (gameData.inventaire.BPAnim.etat == BP_closing)
								{
									gameData.inventaire.BPAnim.time = gameData.inventaire.BPAnim.timeNeeded - gameData.inventaire.BPAnim.time;
								}
								else
								{
									gameData.inventaire.BPAnim.time = 0;
								}
								gameData.inventaire.BPAnim.etat = BP_opening;
							}
							else
							{
								gameData.inventaire.isOpen = sfTrue;
								if (gameData.rotatingBP.bagState == BP_closing)
								{
									for (int i = 0; i < QTITY; i++)
									{
										gameData.rotatingBP.engrenages[i].time = INVENTORYTIME - gameData.rotatingBP.engrenages[i].time;
									}
								}
								gameData.rotatingBP.bagState = BP_opening;
								if (gameData.inventaire.BPAnim.etat == BP_opening)
								{
									gameData.inventaire.BPAnim.time = gameData.inventaire.BPAnim.timeNeeded - gameData.inventaire.BPAnim.time;
								}
								else
								{
									gameData.inventaire.BPAnim.time = 0;
								}
								gameData.inventaire.BPAnim.etat = BP_closing;
							}
						}
					}
				}
				if (!gameData.inventaire.isOpen)
				{
					MovementPlayerEvent(event, gameData.command, &gameData.player, gameData.musics.soundEffect);
				}
				//BP_open backPack
				if (gameData.command.comp2Ncatch.pressed)
				{
					Catch_Loot(&gameData.loot, gameData.player.entity2D.middlePos, &gameData.inventaire.backPack);
				}
				if (gameData.command.inventory.pressed)
				{
					if (gameData.inventaire.current.isMoving == sfFalse)
					{
						if (gameData.inventaire.isOpen)
						{
							gameData.inventaire.isOpen = sfFalse;
#pragma region AnimsBP
							if (gameData.rotatingBP.bagState == BP_opening)
							{
								for (int i = 0; i < QTITY; i++)
								{
									gameData.rotatingBP.engrenages[i].time = INVENTORYTIME - gameData.rotatingBP.engrenages[i].time;
								}
							}
							gameData.rotatingBP.bagState = BP_closing;
							if (gameData.inventaire.BPAnim.etat == BP_closing)
							{
								gameData.inventaire.BPAnim.time = gameData.inventaire.BPAnim.timeNeeded - gameData.inventaire.BPAnim.time;
							}
							else
							{
								gameData.inventaire.BPAnim.time = 0;
							}
							gameData.inventaire.BPAnim.etat = BP_opening;
						}
						else
						{
							gameData.inventaire.isOpen = sfTrue;
							if (gameData.rotatingBP.bagState == BP_closing)
							{
								for (int i = 0; i < QTITY; i++)
								{
									gameData.rotatingBP.engrenages[i].time = INVENTORYTIME - gameData.rotatingBP.engrenages[i].time;
								}
							}
							gameData.rotatingBP.bagState = BP_opening;
							if (gameData.inventaire.BPAnim.etat == BP_opening)
							{
								gameData.inventaire.BPAnim.time = gameData.inventaire.BPAnim.timeNeeded - gameData.inventaire.BPAnim.time;
							}
							else
							{
								gameData.inventaire.BPAnim.time = 0;
							}
							gameData.inventaire.BPAnim.etat = BP_closing;
						}
					}
#pragma endregion
				}

				if (gameData.inventaire.isOpen)
				{
					gameData.player.entity2D.body.vel.x = 0;
					//gachette D
					if (!gameData.inventaire.current.isMoving)
					{
						if (gameData.command.inventoryD.pressed)
						{
							gameData.rotatingBP.etat = BP_selectSlot;
						}

						//Gachette G
						if (gameData.command.inventoryG.pressed)
						{
							gameData.rotatingBP.etat = BP_gestion;
						}
					}



					if (gameData.rotatingBP.etat == BP_gestion)
					{
						MoveBackpackSelect(gameData.command, &gameData.inventaire);
						if (gameData.command.jumpNaction.pressed)
						{
							GestionLeftClickInventory(&gameData.inventaire, gameData.inventaire.selecteur, window);
						}
						//triangle / Y
						if (gameData.command.comp2Ncatch.pressed)
						{
							if (gameData.inventaire.current.isMoving)
							{
								DeleteCurrentItem(&gameData.inventaire);
							}
						}
						//carre / X
						if (gameData.command.atkNequip.pressed)
						{
							gameData.rotatingBP.etat = BP_FastEquip;
						}
					}


					if (gameData.rotatingBP.etat == BP_selectSlot)
					{

						if (gameData.command.jumpNaction.pressed)
						{
							gameData.rotatingBP.etat = BP_selectItem;
						}
					}
					//select slot en temp reel
					else if (gameData.rotatingBP.etat == BP_selectItem)
					{
						MoveBackpackSelect(gameData.command, &gameData.inventaire);

						if (gameData.command.jumpNaction.pressed)
						{
							GestionEquip(&gameData.inventaire, &gameData.rotatingBP, gameData.inventaire.selecteur);
							gameData.rotatingBP.etat = BP_selectSlot;
						}

						if (gameData.command.comp1Nback.pressed)
						{
							gameData.rotatingBP.etat = BP_selectSlot;
						}
					}
					else if (gameData.rotatingBP.etat == BP_FastEquip)
					{
						sfBool test = GestionFastEquip(&gameData.inventaire, &gameData.rotatingBP, gameData.inventaire.selecteur);
						printf("%d\n", test);
						if (test)
						{
							gameData.rotatingBP.etat = BP_Equip;
						}
						else
						{
							gameData.rotatingBP.etat = BP_gestion;
						}
					}
					else if (gameData.rotatingBP.etat == BP_Equip)
					{
						if (gameData.command.jumpNaction.pressed)
						{
							sfVector2i pos = (sfVector2i) { gameData.inventaire.selecteur.x, gameData.inventaire.selecteur.y };

							pos.x = (pos.x - gameData.inventaire.pos.x) / CASESIZE;
							pos.y = (pos.y - gameData.inventaire.pos.y) / CASESIZE;


							if (gameData.inventaire.backPack.bag[pos.x][pos.y].type == gameData.rotatingBP.posSelect.type && gameData.rotatingBP.equiped[gameData.rotatingBP.posSelect.num].itemID != -1)
							{
								BagItem provi = gameData.rotatingBP.equiped[gameData.rotatingBP.posSelect.num];
								gameData.rotatingBP.equiped[gameData.rotatingBP.posSelect.num].itemID = gameData.inventaire.backPack.bag[pos.x][pos.y].itemID;
								gameData.rotatingBP.equiped[gameData.rotatingBP.posSelect.num].type = gameData.inventaire.backPack.bag[pos.x][pos.y].type;
								gameData.rotatingBP.equiped[gameData.rotatingBP.posSelect.num].equipable = gameData.inventaire.backPack.bag[pos.x][pos.y].equipable;

								gameData.inventaire.backPack.bag[pos.x][pos.y].itemID = provi.itemID;
								gameData.inventaire.backPack.bag[pos.x][pos.y].type = provi.type;
								DestroyEquipable(&gameData.inventaire.backPack.bag[pos.x][pos.y].equipable);
							}
							gameData.rotatingBP.etat = BP_gestion;
						}
					}
				}

				if (event.type == sfEvtMouseButtonPressed)
				{
					if (gameData.inventaire.isOpen)
					{
						if (event.mouseButton.button == sfMouseLeft)
						{
							sfVector2f mouse = { event.mouseButton.x ,event.mouseButton.y };
							GestionLeftClickInventory(&gameData.inventaire, mouse, window);
						}
						if (event.mouseButton.button == sfMouseRight)
						{
							if (gameData.inventaire.current.isMoving)
							{
								gameData.inventaire.current.item.itemID = -1;
								gameData.inventaire.current.item.quantity = 0;
								gameData.inventaire.current.isMoving = sfFalse;
							}
						}
					}
					else
					{
						if (event.mouseButton.button == sfMouseLeft)
						{
							Attack(&gameData.player);
						}
					}
				}
			}
			else if (gameData.gameState == menu)
			{
				//pressbutton
				if (gameData.command.jumpNaction.pressed)
				{
					pGameState[gameData.gameState](curseurMenu, &gameData.fade, window);
					if (TestButton((sfVector2f) { 1550, 450 }, (sfVector2f) { 220, 100 }, curseurMenu))
					{
						gameData.instance = ReadMap(0);
						gameData.player = InitPlayer();
						gameData.parallax = InitParallaxPack();
						sfVector2f parallaxMaxPos = (sfVector2f) { (gameData.instance.maps[0].width + 10) * SIZE_TILE, (gameData.instance.maps[0].height + 10) * SIZE_TILE };
						CreateParallax(&gameData.parallax, (sfVector2f) { -SIZE_TILE * 10, -SIZE_TILE * 10 }, (sfVector2f) { parallaxMaxPos.x, parallaxMaxPos.y });
						LoadTiledEnemy(gameData.instance.maps[*indexMap], 1, &gameData.enemyList, &gameData.player.entity2D.middlePos, gameData.enemiesAnimsInfos);
					}
				}
				//move cursor
				if (gameData.command.up.pressed)
				{
					if (curseurMenu.y > 500)
					{
						curseurMenu.y -= 100;
					}
				}

				if (gameData.command.down.pressed)
				{
					if (curseurMenu.y < 800)
					{
						curseurMenu.y += 100;
					}
				}
			}
			if (gameData.gameState == menu)
			{
				if (event.type == sfEvtMouseButtonPressed)
				{
					if (event.mouseButton.button == sfMouseLeft)
					{
						pGameState[gameData.gameState](sfMouse_getPositionRenderWindow(window), &gameData.fade, window);
						if (gameData.gameState == menu)
						{
							gameData.instance = ReadMap(0);
							gameData.player = InitPlayer();
							gameData.parallax = InitParallaxPack();
							sfVector2f parallaxMaxPos = (sfVector2f) { (gameData.instance.maps[0].width + 10) * SIZE_TILE, (gameData.instance.maps[0].height + 10) * SIZE_TILE };
							CreateParallax(&gameData.parallax, (sfVector2f) { -SIZE_TILE * 10, -SIZE_TILE * 10 }, (sfVector2f) { parallaxMaxPos.x, parallaxMaxPos.y });
							LoadTiledEnemy(gameData.instance.maps[*indexMap], 1, &gameData.enemyList, &gameData.player.entity2D.middlePos, gameData.enemiesAnimsInfos);
						}
					}
				}
			}
		}

		// --------------------------------------- == UPDATE == ----------------------------------------//
		//----------------------------------------------------------------------------------------------\\

		if (gameData.gameState == jeu)
		{
			// ------------------------------------- == HUD UPDATE == --------------------------------------//

			//----------------------------------------------------------------------------------------------\\
				


			mousePerma = sfMouse_getPositionRenderWindow(window);
			UpdateHud(&gameData.HudDatas, gameData.player, deltaTime);

			if (gameData.rotatingBP.bagState == BP_open && (gameData.rotatingBP.etat == BP_selectSlot || gameData.rotatingBP.etat == BP_Equip))
			{
				gameData.rotatingBP.posSelect = getGearNumber(gameData.rotatingBP, gameData.rotatingBP.posSelect, window);
			}



			mousePerma = sfMouse_getPositionRenderWindow(window);

			//UPDATE HUD
			UpdateHudGears(gameData.hudAnim.angle, gameTime, deltaTime);
			UpdateEngrenage(&gameData.inventaire.BPAnim, deltaTime, 1);
			sfSprite_setRotation(gameData.spritePack.inventorySprites.BPSprites.engrenage, (gameData.inventaire.BPAnim.pos.y - gameData.inventaire.BPAnim.posengrenageInit.y) * 15 / 17);



			//set correct pos of the quadrilage of the inventory
			gameData.inventaire.pos = AddSubInt(gameData.inventaire.BPAnim.pos, 13, 1);

			//init pos selecteur 
			if (gameData.rotatingBP.bagState == BP_open)
			{
				if (posSelect.x == 0 && posSelect.y == 0)
				{
					posSelect = gameData.rotatingBP.engrenages[0].pos;
				}
			}
			RotatingBPPointsUpdate(gameData.rotatingBP.engrenages, deltaTime, gameTime, &gameData.rotatingBP.bagState);

			//printf("gameData.player.stats.dir : %d\n", gameData.player.stats.dir);

			//if (gameData.rotatingBP.bagState != close && gameData.rotatingBP.bagState != closing)
			//{
			//	SetPosCamera(&gameData.camera, gameData.player.entity2D.body.pos, 0, deltaTime);
			//}
			//else
			//{
			//	SetPosCamera(&gameData.camera, gameData.player.entity2D.body.pos, gameData.player.entity2D.dir, deltaTime);
			//}
			pFunction[gameData.player.state](&gameData.player);

			spellFunction[gameData.player.spell[0].name](&gameData.player.spell[0], &gameData.player.stats, &gameData.player.entity2D, &gameData.projectileList, deltaTime);
			spellFunction[gameData.player.spell[1].name](&gameData.player.spell[1], &gameData.player.stats, &gameData.player.entity2D, &gameData.projectileList, deltaTime);

			UpdateInstance(&gameData.enemyList, &gameData.loot, &gameData.player, gameData.enemiesAnimsInfos, &gameData.instance, &gameData.fade, &gameData.parallax);

			sfVector2i mapSize = (sfVector2i) { gameData.instance.maps[*indexMap].width, gameData.instance.maps[*indexMap].height };
			UpdateParallax(&gameData.parallax, &gameData.camera, gameData.player, mapSize, deltaTime);
			SetPosCamera(&gameData.camera, gameData.player, mapSize, deltaTime);

			// sfKeyboard
			ControlCamera(&gameData.camera, deltaTime);
			if (gameData.rotatingBP.bagState == BP_close)
			{
				MovementPlayer(&gameData.player, gameData.command);
			}

			UpdatePlayer(&gameData.player, &gameData.spritePack, deltaTime);
			UpdateEntity(&gameData.player.entity2D, deltaTime);
			Entity2DMapCollision(&gameData.player.entity2D, gameData.collisionMap, gameData.instance, *indexMap);

			UpdateEnemies(&gameData.enemyList, &gameData.projectileList, &gameData.loot, gameData.collisionMap, gameData.instance, deltaTime);

			UpdateParticles(&gameData.particle, deltaTime);


			// PARTICLE WHEN PLAYER RUNNING TEST
			if (gameData.player.state == pla_walking)
			{
				gameData.player.timer.runningParticle += deltaTime;
				if (gameData.player.timer.runningParticle > 0.1)
				{
					CreateParticles(&gameData.particle, dust, 1, (sfVector2f) { gameData.player.entity2D.body.pos.x - 50 * gameData.player.entity2D.dir, gameData.player.entity2D.body.pos.y });
					gameData.player.timer.runningParticle = 0;
				}
			}
			if (gameData.player.state == pla_impulsion || gameData.player.state == pla_landing)
			{
				int tmpRand = rand() % 100 - 50;
				CreateParticles(&gameData.particle, quickplosion, 1, (sfVector2f) { gameData.player.entity2D.body.pos.x + tmpRand, gameData.player.entity2D.body.pos.y });
			}

			// ------------------------------------ == Projectiles Update == -------------------------------------//

			//---------------------------------------------------------------------------------------------\\

			UpdateProjectiles(&gameData.projectileList, deltaTime);

			// ------------------------------------ == FightBox Update == -------------------------------------//

			//---------------------------------------------------------------------------------------------\\

			UpdateFightBox(gameData.fightBoxPack, &gameData.player, &gameData.enemyList, &gameData.projectileList, gameData.spritePack);

			//printf("%f %f\n", gameData.player.entity2D.body.pos.x, gameData.player.entity2D.body.pos.y);
		}
		else if (gameData.gameState == menu)
		{

		}
		else if (gameData.gameState == creajeux)
		{
			timerLogo += deltaTime;
			if (timerLogo > 3.f)
			{
				gameData.fade.infos.changeTo = menu;
				gameData.fade.infos.state = F_fadeIn;
			}
		}


		// ------------------------------------ == FADE Update == -------------------------------------//

		//---------------------------------------------------------------------------------------------\\

		UpdateFade(&gameData.fade, &gameData.gameState, deltaTime);

		// --------------------------------------- == RENDER == ----------------------------------------//

		//----------------------------------------------------------------------------------------------\\

		sfRenderWindow_clear(window, sfWhite);

		if (gameData.gameState == jeu)
		{
			sfRenderWindow_setView(window, sfRenderWindow_getDefaultView(window));

			//BlitSprite(gameData.spritePack.parallax.background[0], (sfVector2f) { 0, 0 }, window);
			RenderBackground(window, gameData.spritePack);

			//RENDER VIEW Camera
			sfRenderWindow_setView(window, gameData.camera.view);

			//RENDER MAP
			RenderMapInstance(gameData.camera, gameData.mapSprites, gameData.layers, gameData.spritePack, gameData.instance, *indexMap);

			//RENDER PLAYER
			RenderPlayer(gameData.spritePack, gameData.player, gameData.layers, window);

			//RENDER ENEMIES
			RenderEnemies(gameData.spritePack, &gameData.enemyList, gameData.layers);

			//RENDER PROJECTILES
			RenderProjectiles(gameData.spritePack, &gameData.projectileList, gameData.layers);

			//RENDER PARALLAX
			RenderParallax(window, &gameData.parallax, gameData.camera, gameData.spritePack, gameData.layers);

			//RENDER PARTICULE
			RenderParticles(&gameData.layers, &gameData.particle, gameData.spritePack);

			//RENDER LAYERS
			RenderLayers(gameData.layers, window);


#pragma region //Render DEBUG
			//float angle = atan2f(gameData.player.entity2D.moveDir.y * gameData.player.entity2D.dir, gameData.player.entity2D.moveDir.x * gameData.player.entity2D.dir) * 180.f / PI;
			//sfSprite_setRotation(flecheDir, angle);
			//BlitSprite(flecheDir, gameData.player.entity2D.bodyCollider.topC.center, window);


			//sfCircleShape_setPosition(headCircle, gameData.player.entity2D.bodyCollider.topC.center);
			//sfVector2f rectPos = gameData.player.entity2D.body.pos;
			//rectPos.y -= 242.0f / 2;
			//sfRectangleShape_setPosition(rectPlayer, rectPos);

			//for (int i = 0; i < 3; i++)
			//{
			//sfCircleShape_setPosition(legsCircle, gameData.player.entity2D.bodyCollider.bottomPoints[i]);
			//sfRenderWindow_drawCircleShape(window, legsCircle, NULL);
			////printf("i:%d->x:%f\ty:%f\n",i, gameData.player.entity2D.bodyCollider.bottomPoints[i].x, gameData.player.entity2D.bodyCollider.bottomPoints[i].y);
			//}

			//sfRenderWindow_drawCircleShape(window, headCircle, NULL);
			//sfRenderWindow_drawRectangleShape(window, rectPlayer, NULL);


			//RENDER FIGHTBOX

			/*FightBox tmpFightBox = gameData.fightBoxPack.player[gameData.player.state][gameData.player.anim[gameData.player.state].actualImage];
			sfVector2f OriginSprite = sfSprite_getOrigin(gameData.spritePack.player[gameData.player.state].sprite[gameData.player.anim[gameData.player.state].actualImage]);
			RenderFightBoxDebug(tmpFightBox, gameData.player.entity2D.body.pos, OriginSprite, gameData.player.entity2D.dir, window);*/

			//RENDER COLLISION
			//RenderMapCollider(gameData.camera, gameData.instance, gameData.collisionMap, window);
#pragma endregion

			//RENDER VIEW Default
			sfRenderWindow_setView(window, sfRenderWindow_getDefaultView(window));

			//RENDER HUD
			BlitHudGears(gameData.hudAnim, window);
			BlitHud(gameData.HudDatas, window);
			InventoryDisplay(gameData.inventaire, gameData.rotatingBP, gameData.inventaire.BPAnim, gameData.spritePack, &text, window);

			BlitGears(gameData.rotatingBP, gameData.rotatingBP.posSelect, gameData.spritePack.inventorySprites.rotattingBPSprite, gameData.spritePack.inventorySprites.BPSprites, window);
			BlitSprite(gameData.spritePack.inventorySprites.rotattingBPSprite.engrenages[0], gameData.rotatingBP.posObjets[0], window);
			Render_Loot(gameData.loot, gameData.layers, gameData.spritePack.items);





			//RENDER FPS

			/*char fpsChar[20];
			sprintf(fpsChar, "fps:%.0f", 1 / deltaTime);
			BlitText(&gameData.fpsText, (sfVector2f) { 10, 10 }, 50, fpsChar, sfWhite, window);*/
			//BlitText(&text, (sfVector2f) { 280, 50 }, 90, "JEU FONCTIONNEL ne pas rager svp", sfRed, window);

		}
		else if (gameData.gameState == menu)
		{
			//RENDER VIEW Default
			sfRenderWindow_setView(window, sfRenderWindow_getDefaultView(window));

			BlitSprite(menuSprite, (sfVector2f) { 0, 0 }, window);

			BlitSprite(gameData.spritePack.cursorMenu, (sfVector2f) { curseurMenu.x - 150, curseurMenu.y }, window);

			BlitText(&text, (sfVector2f) { 1550, 450 }, 75, "PLAY", sfWhite, window);
			BlitText(&text, (sfVector2f) { 1550, 550 }, 75, "OPTION", sfWhite, window);
			BlitText(&text, (sfVector2f) { 1550, 650 }, 75, "CREDITS", sfWhite, window);
			BlitText(&text, (sfVector2f) { 1550, 750 }, 75, "EXIT", sfWhite, window);
		}
		else if (gameData.gameState == options)
		{

		}
		else if (gameData.gameState == credits)
		{

		}
		else if (gameData.gameState == creajeux)
		{
			//RENDER VIEW Default
			sfRenderWindow_setView(window, sfRenderWindow_getDefaultView(window));

			BlitSprite(logoCrea, VEC2F_NULL, window);
		}

		if (gameData.fade.infos.state != F_inactive)
		{
			RenderFade(gameData.fade, window);
		}

		sfRenderWindow_display(window);
	}

	return 0;
}
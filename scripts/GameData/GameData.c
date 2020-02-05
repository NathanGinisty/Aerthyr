#include "gameData.h"

GameData LoadGameData()
{
	GameData gd;

	//___________________ Init Map ___________________//
	//tempGameData.utils = LoadUtils();
	gd.collisionMap = MapCollisionLoader(0);
	gd.mapSprites = LoadMapSprites();
	/*gd.instance = ReadMap(0);*/


	//_____________ Init Player et Ennemis _____________//
	gd.player = InitPlayer();
	gd.fightBoxPack = LoadFightBox();
	gd.enemyList = NULL;
	gd.projectileList = NULL;


	//_____________ Init Objets Graphiques _____________//
	for (int i = 0; i < NB_LAYERS; i++)
	{
		gd.layers[i].premier = NULL;
	}
	gd.camera = LoadCamera(gd.player.entity2D.body.pos);
	//gd.parallax = InitParallaxPack();
	gd.particle = InitParticle();
	gd.fade = LoadFade();


	//_________ Init Inventaire + HUD et Loot _________//
	gd.HudDatas = InitHudDatas();
	gd.hudAnim = InitHudGears();
	gd.inventaire = InitInventory();
	gd.rotatingBP = initRotatingBackpack();
	initPosGears(&gd.rotatingBP);
	gd.inventaire.BPAnim = InitBackpackAnim();
	gd.loot;
	gd.loot.first = NULL;


	//___________________ Init Sons ___________________//
	gd.musics.menu = iniMusique("Sound/menuMusic.wav", sfTrue);
	gd.musics.inGame = iniMusique("Sound/inGameMusic.wav", sfTrue);
	gd.musics.soundEffect = initSoundTab(1);
	gd.musics.soundEffect.son[0] = initSound("Sound/swish.wav", sfFalse);
	

	//__________________ Init Outils __________________//
	CreateText(&gd.fpsText, "Font/BebasNeue-Regular.ttf");
	gd.command = InitCommand();
	gd.gameState = creajeux;


	//________________ Init SpritePack ________________//
	gd.spritePack.player = LoadPlayerSprite(gd.player);

	gd.spritePack.inventorySprites.rotattingBPSprite = LoadRotatingBPSprite();
	gd.spritePack.inventorySprites.BPSprites = LoadBPSprite();
	gd.spritePack.items = InitItemSprites();
	gd.spritePack.cursorMenu = LoadSprite("characursor.png", 1);

	gd.spritePack.parallax = LoadParallaxSprite(1);
	gd.spritePack.particle = LoadParticleSprite();

	gd.spritePack.projectiles[Proj_Ranger] = LoadSprite("Assets/Projectiles/Enemies/Cast_noisette.png", 1);
	gd.spritePack.projectiles[Proj_Player_FireBall] = LoadSprite("Assets/Projectiles/Player/patatoes.png", 1);

	gd.spritePack.enemiesSprites = LoadEnemiesAnim(&gd.enemiesAnimsInfos);

	gd.spritePack.tileImage = LoadTileSprite();
	return gd;
}
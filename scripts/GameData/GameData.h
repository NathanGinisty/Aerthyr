#ifndef GAMEDATA_H
#define GAMEDATA_H
#include "../stdafx.h"
#include "../Backpack/BackPack.h"
#include "../HUD/HUD.h"
#include "../FightBox/fightBox.h"
#include "../InstanceManager/InstanceManager.h"

#define DEFAULTOPTIONS (GameOptions){{100,100,100}, {1920, 1080}, sfDefaultStyle}

typedef enum
{
	Window = sfDefaultStyle,
	Borderless = sfNone,
	Fullscreen = sfFullscreen
}WindowStyle;

typedef struct
{
	int master;
	int music;
	int sounds;
}Volume;

typedef struct
{
	Volume volume;
	sfVector2i resolution;
	WindowStyle windowStyle;
}GameOptions;

typedef struct
{
	//Utils utils;
	GameOptions option;
	CollisionMap collisionMap;
	MapSprites mapSprites;
	CameraViews camera;
	InstanceMaps instance;
	Player player;
	struct Enemy* enemyList;
	struct Projectile* projectileList;
	EnemiesAnimsInfos enemiesAnimsInfos;
	SpritePack spritePack;
	ParallaxObjectPack parallax;
	Layers layers[NB_LAYERS];
	TextInfo fpsText;
	HUDStruct HudDatas;
	BackpackDatas inventaire;
	RotatingBackpackDatas rotatingBP;
	HUDGearsAnim hudAnim;
	BackpackAnim BPAnim;
	Fade fade;
	GameState gameState;
	ParticleEffect particle;
	FightBoxPack fightBoxPack;
	GameMusics musics;
	Command command;
	Loot_Item loot;
}GameData;

GameData LoadGameData();

#endif // !GAMEDATA_H
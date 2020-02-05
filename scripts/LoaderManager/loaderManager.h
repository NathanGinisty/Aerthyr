#ifndef LOADERMANAGER_H
#define LOADERMANAGER_H

#include "../stdafx.h"
#include "../GameData/GameData.h"
#include <Dwmapi.h>
#pragma comment (lib, "Dwmapi.lib")

typedef struct
{
	GameData* gameData;
	sfBool* endThead;
}DataThread1;


//display the GameLogo when game launch
void LoaderDisplay(DataThread1* data, float deltaTime, sfClock* dtClock);

//load gameData
void ChargementGameDataThread(DataThread1* data);

#endif // !LOADERMANAGER_H
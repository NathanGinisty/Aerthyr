#ifndef INSTANCEMANAGER_H
#define INSTANCEMANAGER_H
#include "../stdafx.h"
#include "../EnemyController/EnemyController.h"
#include "../Parallax/parallax.h"
#include "../MenuManager/MenuManager.h"

void UpdateInstance(struct Enemy** enemies, Loot_Item* itemList, Player* player, EnemiesAnimsInfos e_AnimsInfos, InstanceMaps* instMaps, Fade* fade, ParallaxObjectPack* paralax);
void RenderMapCollider(CameraViews camera, InstanceMaps instance, CollisionMap collisionMap, sfRenderWindow* window);
void RenderMapInstance(CameraViews camera, MapSprites mapSprites, Layers layers[], SpritePack spritePack, InstanceMaps instance, int indexMap);
#endif //!INSTANCEMANAGER_H
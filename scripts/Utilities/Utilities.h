#ifndef UTILITIES_H
#define UTILITIES_H
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "SFML\Graphics.h"

#define SIZE_TILE 128 


#define SCREENW 1920
#define SCREENH 1080

#define PI 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825f


#define RAD2DEG 180.f/PI
#define DEG2RAD PI/180.f

#define RIGHT 1
#define LEFT -1

#define VEC2F_NULL (sfVector2f){ 0 ,0 }
#define VEC2I_NULL (sfVector2i){ 0 ,0 }

void BlitSprite(sfSprite* sprite, sfVector2f pos, sfRenderWindow* window);
void BlitSegment(sfVector2f pointA, sfVector2f pointB, sfColor color, sfRenderWindow* window);
void BlitRectShape(sfRectangleShape* rect, sfVector2f pos, sfColor color, sfRenderWindow* window);

sfSprite* LoadSprite(char* _sNom, char _isCentered);

sfVector2f lerpVec2f(sfVector2f A, sfVector2f B, float actualTime, float neededTime);
sfVector2f moveVec2f(sfVector2f actualPos, sfVector2f futurePos, float spd, float dt);

float Anglevect(sfVector2f u, sfVector2f v);
sfBool IsSameSign(float a, float b);

int testPosDansCarre(sfVector2f pos, int size, sfVector2f pos2);

#endif // !UTILITIES_H
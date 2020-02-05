#ifndef MATHELEMENTS_H
#define MATHELEMENTS_H
#include "../stdafx.h"
#include "../Utilities/Utilities.h"

typedef struct
{
	sfVector2f center;
	float radius;
}Circle;

typedef struct
{
	sfVector2f point;
	sfVector2f dir;
}StraightLine;

typedef struct
{
	sfVector2f pointA;
	sfVector2f pointB;
	sfVector2f dir;
}SegmentLine;

float DegrToRad(float angle);

float getDistance(sfVector2f A, sfVector2f B);

float GetAxeDistance(float a, float b);

sfVector2f GetDiffVector2f(sfVector2f A, sfVector2f B);

sfVector2f AdditionVec2f(sfVector2f A, sfVector2f B);

sfVector2f multiplyVec2f(sfVector2f A, sfVector2f B);

sfVector2f Vec2fPerp(sfVector2f vec);

float Magnitude(sfVector2f v);

float Dot(sfVector2f u, sfVector2f v);

sfVector2f Normalize(sfVector2f v);


// 0 -> soustraction
// 1 -> addition
sfVector2f AddSub2f(sfVector2f a, sfVector2f b, int type);

// 0 -> division
// 1 -> multiplication
sfVector2f MultDiv2f(sfVector2f a, sfVector2f b, int type);

// 0 -> soustraction
// 1 -> addition
sfVector2f AddSubInt(sfVector2f a, int b, int type);

// 0 -> division
// 1 -> multiplication
sfVector2f MultDivInt(sfVector2f a, int b, int type);

// 0 -> division
// 1 -> multiplication
sfVector2f MultDivFloat(sfVector2f a, float b, int type);

sfVector2f Rotatage(sfVector2f v, float angle);

sfVector2f RotatageAutour(sfVector2f v, sfVector2f pivot, float angle);

sfVector2f ShiftDecal(sfVector2f pos, sfVector2f decal);


#endif // !MATHELEMENTS_H
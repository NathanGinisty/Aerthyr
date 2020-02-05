#include "Utilities.h"

sfVector2f lerpVec2f(sfVector2f A, sfVector2f B, float actualTime, float neededTime)
{
	sfVector2f vec2 = A;
	float t = actualTime / neededTime;

	vec2.x += (B.x - A.x) * t;
	vec2.y += (B.y - A.y) * t;

	return vec2;
}

sfVector2f moveVec2f(sfVector2f actualPos, sfVector2f futurePos, float spd, float dt)
{
	sfVector2f result;

	sfVector2f direction;
	direction.x = futurePos.x - actualPos.x;
	direction.y = futurePos.y - actualPos.y;

	float length = sqrtf(direction.x * direction.x + direction.y * direction.y);

	if (length > 1.0f)
	{
		direction.x /= length;
		direction.y /= length;
	}
	result.x = actualPos.x + direction.x * spd * dt;
	result.y = actualPos.y + direction.y * spd * dt;

	return result;
}


void BlitSprite(sfSprite* sprite, sfVector2f pos, sfRenderWindow* window)
{
	sfSprite_setPosition(sprite, pos);
	sfRenderWindow_drawSprite(window, sprite, NULL);
}

void BlitSegment(sfVector2f pointA, sfVector2f pointB, sfColor color, sfRenderWindow* window)
{
	sfVertexArray* tempVertexArray;
	sfVertex tempVertex;

	tempVertexArray = sfVertexArray_create();
	sfVertexArray_resize(tempVertexArray, 2);
	sfVertexArray_setPrimitiveType(tempVertexArray, sfLines);

	tempVertex.position = pointA;
	tempVertex.color = sfRed;
	sfVertexArray_append(tempVertexArray, tempVertex);

	tempVertex.position = pointB;
	tempVertex.color = sfBlue;
	sfVertexArray_append(tempVertexArray, tempVertex);

	sfRenderWindow_drawVertexArray(window, tempVertexArray, NULL);

	sfVertexArray_clear(tempVertexArray);
	sfVertexArray_destroy(tempVertexArray);
}

void BlitRectShape(sfRectangleShape* rect, sfVector2f pos, sfColor color, sfRenderWindow* window)
{
	sfRectangleShape_setPosition(rect, pos);
	sfRectangleShape_setFillColor(rect, color);

	sfRenderWindow_drawRectangleShape(window, rect, NULL);
}

sfSprite* LoadSprite(char* _sNom, char _isCentered)
{
	sfTexture* tempTex;
	sfSprite* tempSprite;
	tempTex = sfTexture_createFromFile(_sNom, NULL);

	tempSprite = sfSprite_create();
	sfSprite_setTexture(tempSprite, tempTex, 0);

	if (_isCentered)
	{
		sfVector2u tailleImage = sfTexture_getSize(tempTex);
		sfVector2f centre = { tailleImage.x / 2.f, tailleImage.y / 2.f };
		sfSprite_setOrigin(tempSprite, centre);
	}
	return tempSprite;
}

float Anglevect(sfVector2f u, sfVector2f v)
{
	float scalaire = { u.x * v.x + u.y * v.y };
	float normeU = sqrtf(u.x * u.x + u.y * u.y);
	float normeV = sqrtf(v.x * v.x + v.y * v.y);

	float angle = scalaire / (normeU * normeV);
	float ret = acosf(angle) * RAD2DEG;
	return ret;
}

sfBool IsSameSign(float a, float b)
{
	return (a < 0.0f && b < 0.0f) || (a >= 0.0f && b >= 0.0f);
}

int testPosDansCarre(sfVector2f pos, int size, sfVector2f pos2)
{
	int test1 = pos2.x > pos.x && pos2.x < pos.x + size;
	int test2 = pos2.y > pos.y && pos2.y < pos.y + size;

	return test1 && test2;
}


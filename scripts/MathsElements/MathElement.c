#include "MathElements.h"

#define PI 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825f

float DegrToRad(float angle)
{
	return PI * angle / 180;
}

float getDistance(sfVector2f A, sfVector2f B)
{
	sfVector2f dif;
	dif.x = B.x - A.x;
	dif.y = B.y - A.y;
	float d = sqrtf(dif.x * dif.x + dif.y * dif.y);
	return d;
}

float GetAxeDistance(float a, float b)
{
	float dif;
	float d;
	dif = b - a;
	d = sqrtf(dif*dif);

	return d;
}

sfVector2f GetDiffVector2f(sfVector2f A, sfVector2f B)
{
	sfVector2f diff;
	diff.x = A.x - B.x;
	diff.y = A.y - B.y;
	return diff;
}


sfVector2f AdditionVec2f(sfVector2f A, sfVector2f B)
{
	sfVector2f add;
	add.x = A.x + B.x;
	add.y = A.y + B.y;
	return add;
}

sfVector2f multiplyVec2f(sfVector2f A, sfVector2f B)
{
	sfVector2f add;
	add.x = A.x * B.x;
	add.y = A.y * B.y;
	return add;
}

sfVector2f Vec2fPerp(sfVector2f vec)
{
	sfVector2f perp = { vec.y,-vec.x };
	return perp;
}

float Magnitude(sfVector2f v)
{
	return sqrtf(v.x * v.x + v.y * v.y);
}

//x.x'+y.y'
float Dot(sfVector2f u, sfVector2f v)
{
	return u.x * v.x + u.y * v.y;
}

sfVector2f Normalize(sfVector2f v)
{
	float length = Magnitude(v);
	v.x /= length;
	v.y /= length;
	return v;
}



//fonction gestion vecteur
// 0 -> soustraction
// 1 -> addition
sfVector2f AddSub2f(sfVector2f a, sfVector2f b, int type)
{
	sfVector2f ret = a;
	if (type == 0)
	{
		ret.x -= b.x;
		ret.y -= b.y;
	}
	else
	{
		ret.x += b.x;
		ret.y += b.y;
	}
	return ret;
}

// 0 -> division
// 1 -> multiplication
sfVector2f MultDiv2f(sfVector2f a, sfVector2f b, int type)
{
	sfVector2f ret = a;
	if (type == 0)
	{
		ret.x /= b.x;
		ret.y /= b.y;
	}
	else
	{
		ret.x *= b.x;
		ret.y *= b.y;
	}
	return ret;
}

// 0 -> soustraction
// 1 -> addition
sfVector2f AddSubInt(sfVector2f a, int b, int type)
{
	sfVector2f ret = a;
	if (type == 0)
	{
		ret.x -= b;
		ret.y -= b;
	}
	else
	{
		ret.x += b;
		ret.y += b;
	}
	return ret;
}

// 0 -> division
// 1 -> multiplication
sfVector2f MultDivInt(sfVector2f a, int b, int type)
{
	sfVector2f ret = a;
	if (type == 0)
	{
		ret.x /= b;
		ret.y /= b;
	}
	else
	{
		ret.x *= b;
		ret.y *= b;
	}
	return ret;
}

// 0 -> division
// 1 -> multiplication
sfVector2f MultDivFloat(sfVector2f a, float b, int type)
{
	sfVector2f ret = a;
	if (type == 0)
	{
		ret.x /= b;
		ret.y /= b;
	}
	else
	{
		ret.x *= b;
		ret.y *= b;
	}
	return ret;
}


sfVector2f Rotatage(sfVector2f v, float angle)
{
	sfVector2f vOut;
	float cosA = cosf(angle);
	float sinA = sinf(angle);

	vOut.x = v.x * cosA - v.y * sinA;
	vOut.y = v.y * cosA + v.x * sinA;

	return vOut;
}

sfVector2f RotatageAutour(sfVector2f v, sfVector2f pivot, float angle)
{
	v.x -= pivot.x;
	v.y -= pivot.y;

	v = Rotatage(v, angle);

	v.x += pivot.x;
	v.y += pivot.y;

	return v;
}

sfVector2f ShiftDecal(sfVector2f pos, sfVector2f decal)
{
	return AddSub2f(pos, decal, 1);
}

#ifndef PHYSICS_H
#define PHYSICS_H

#define GRAVITY 5800.f
#include "../stdafx.h"
#include "../MapLoader/MapLoader.h"

typedef enum
{
	nothing,
	rebound
}SegmentProperty;

typedef enum
{
	Top,
	Bottom,
	Left,
	Right
}CollisionZone;

// ----- STRUCT -----//

typedef struct
{
	sfVector2f pos;
	sfVector2f vel;
	sfVector2f accel;
	sfVector2f force;
	float Mass;
	sfBool Enablegravity;
}rigidBody;

typedef struct
{
	sfBool isCollided;
	sfVector2f point;
	sfVector2f normal;
}Hit;

typedef struct
{
	SegmentLine *line;
	SegmentProperty *property;
	int nbSegment;
}CollisionSegment;

typedef struct
{
	Circle topC;
	sfFloatRect rect;
	sfVector2f bottomPoints[3];
}BodyCollider;

typedef struct
{
	rigidBody body;
	BodyCollider bodyCollider;
	sfVector2i previousBlocPos;
	sfVector2i blocPos;
	sfVector2f moveDir;
	sfVector2f middlePos;
	sfBool moveDirActive;
	sfBool needDirUpdate;
	sfVector2u size;
	int blocID;
	int previousBlocID;
	sfBool isOnGround;
	sfBool hasToCheckGround;
	sfBool hasHitWall;
	int dir;
}Entity2D;

// ----- FUNCTION -----//

rigidBody RigidbodyLoader(sfVector2f pos, float mass, sfBool enableGravity);

void RigidBodyUpdate(rigidBody *rb, float dt);

void UpdateEntity(Entity2D* entity, float dt);

Hit IsCollindingLineCircle(StraightLine l, Circle c);

Hit IsCollindingSegmentCircle(SegmentLine l, Circle c);

sfBool IsCollindingPointCircle(sfVector2f point, Circle circle);

Hit IsCollindingCircleCircle(Circle A, Circle B);

sfBool IsCollindingPointLine(sfVector2f point, StraightLine line);

int intersectsegment(sfVector2f A, sfVector2f B, sfVector2f I, sfVector2f P);

sfBool CollisionSegSeg(sfVector2f A, sfVector2f B, sfVector2f O, sfVector2f P);

sfBool IsCollidingSegSeg(sfVector2f A, sfVector2f B, sfVector2f O, sfVector2f P);

sfBool isCollidingPoly(sfVector2f point, MapPrefab poly, sfVector2i posMap);

sfBool isCollidingRectRect(sfFloatRect A, sfFloatRect B);

void SetEntityCollidingPos(Entity2D* entity, Circle* circleCollider, Hit hit, sfBool resetVelY);

Hit CapsCircleCollision(Circle circleCollider, MapPrefab poly, sfVector2i posMap, sfBool groundBlock, CollisionZone zone);

void BodyCollisionTop(Entity2D* entity, CollisionMap collisionMap, InstanceMaps instanceMaps, int indexMap);

sfBool IsCollindingPointRect(sfVector2f point, sfFloatRect rect);

sfVector2f BodyCollisionSide(BodyCollider* collider, CollisionMap collisionMap, InstanceMaps instanceMaps, int indexMap, int dirX);

int GetGroundSegmentID(CollisionMap map, int colliderID);

void UpdateEntity2DColl(Entity2D* entity2D);

Hit BodyCollisionBottom(Entity2D* entity, CollisionMap collisionMap, InstanceMaps instanceMaps, int indexMap, int pointID);

void Entity2DMapCollision(Entity2D* entity2D, CollisionMap collisionMap, InstanceMaps instanceMaps, int indexMap);
#endif // !PHYSICS_H
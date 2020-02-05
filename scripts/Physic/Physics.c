#include "physics.h"
/////////////////////////////////////////////////////////
/*						IDEA

check pour le passage en ground false
direction de deplacement que sur les pentes ?
remettre le blockID de entity pour le check du sol


*/
/////////////////////////////////////////////////////////

rigidBody RigidbodyLoader(sfVector2f pos, float mass, sfBool enableGravity)
{
	rigidBody tempBody =
	{
		50, 50,
		0.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 0.0f,
		1.0f,
		sfFalse
	};
	tempBody.pos = pos;
	tempBody.Mass = mass;
	tempBody.Enablegravity = enableGravity;

	return tempBody;
}

void RigidBodyUpdate(rigidBody *rb, float dt)
{
	rb->accel.x = rb->force.x / rb->Mass;
	rb->accel.y = rb->force.y / rb->Mass;

	if (rb->Enablegravity)
	{
		rb->accel.y += GRAVITY;
	}

	rb->pos.x += rb->vel.x * dt + (0.5f * rb->accel.x * dt *dt);
	rb->pos.y += rb->vel.y * dt + (0.5f * rb->accel.y * dt *dt);

	rb->vel.x += rb->accel.x * dt;
	rb->vel.y += rb->accel.y * dt;

	rb->force.x = 0;
	rb->force.y = 0;
}

void UpdateEntity2DColl(Entity2D* entity)
{
	float offsetX = entity->size.x / 2;

	entity->bodyCollider.topC.center.x = entity->body.pos.x;
	entity->bodyCollider.topC.center.y = entity->body.pos.y - entity->size.y + offsetX;

	entity->bodyCollider.rect.top = entity->bodyCollider.topC.center.y;
	entity->bodyCollider.rect.left = entity->body.pos.x - (offsetX);

	entity->middlePos.x = entity->body.pos.x;
	entity->middlePos.y = entity->body.pos.y - (entity->size.y / 2.f);

	int index = 0;
	for (int i = -1; i < 2; i++)
	{
		entity->bodyCollider.bottomPoints[index].x = entity->body.pos.x + (entity->size.x / 2.f)*i;
		entity->bodyCollider.bottomPoints[index].y = entity->body.pos.y;
		index++;
	}
}

void UpdateEntity(Entity2D* entity, float dt)
{
	RigidBodyUpdate(&entity->body, dt);
	UpdateEntity2DColl(entity);

	entity->middlePos.x = entity->body.pos.x;
	entity->middlePos.y = entity->body.pos.y - entity->size.y / 2.f;
}

// --------------------------------------- == COLLIDING TEST == ----------------------------------------//

//------------------------------------------------------------------------------------------------------\\

Hit IsCollindingLineCircle(StraightLine l, Circle c)
{
	sfVector2f v = { c.center.x - l.point.x , c.center.y - l.point.y };
	sfVector2f perp = { l.dir.y,-l.dir.x };
	float dot = Dot(v, perp);

	Hit hit;
	hit.normal = perp;
	hit.point.x = c.center.x - perp.x * dot;
	hit.point.y = c.center.y - perp.y * dot;

	hit.isCollided = (fabsf(dot) <= c.radius);

	return hit;
}

Hit IsCollindingSegmentCircle(SegmentLine l, Circle c)
{
	Hit hit;
	StraightLine tempLine = { l.pointA , l.dir };
	sfVector2f v = { c.center.x - l.pointA.x , c.center.y - l.pointA.y };
	sfVector2f perp = { l.dir.y,-l.dir.x };
	sfVector2f AB = { l.pointB.x - l.pointA.x , l.pointB.y - l.pointA.y };
	float dot = Dot(v, perp);

	hit = IsCollindingLineCircle(tempLine, c);
	if (hit.isCollided)
	{
		float dotLenght = Dot(v, l.dir);
		float lenght = Magnitude(AB);
		if (dotLenght < 0)
		{
			if (IsCollindingPointCircle(l.pointA, c))
			{
				hit.normal = Normalize(v);
				hit.point = l.pointA;
				hit.isCollided = sfTrue;
			}
			else
			{
				hit.isCollided = sfFalse;
			}
		}
		else if (dotLenght <= lenght)
		{
			hit.normal = perp;
			hit.point.x = c.center.x - perp.x * dot;
			hit.point.y = c.center.y - perp.y * dot;
			hit.isCollided = sfTrue;
		}
		else
		{
			if (IsCollindingPointCircle(l.pointB, c))
			{
				sfVector2f v = { c.center.x - l.pointB.x , c.center.y - l.pointB.y };
				hit.normal = Normalize(v);
				hit.point = l.pointB;
				hit.isCollided = sfTrue;
			}
			else
			{
				hit.isCollided = sfFalse;
			}
		}
	}

	return hit;
}

sfBool IsCollindingPointCircle(sfVector2f point, Circle circle)
{
	return getDistance(point, circle.center) < circle.radius;
}

Hit IsCollindingCircleCircle(Circle A, Circle B)
{
	Hit hit;
	hit.isCollided = getDistance(A.center, B.center) < A.radius + B.radius;
	hit.normal = GetDiffVector2f(A.center, B.center);
	hit.normal = Normalize(hit.normal);
	hit.point.x = hit.normal.x*B.radius;
	hit.point.y = hit.normal.y*B.radius;
	hit.point = AdditionVec2f(hit.point, B.center);
	return hit;
}

sfBool IsCollindingPointLine(sfVector2f point, StraightLine line)
{
	sfVector2f v = { point.x - line.point.x , point.y - line.point.y };
	sfVector2f perp = { line.dir.y,-line.dir.x };
	return (Dot(v, perp) <= 0);
}

sfBool IsCollidingSegSeg(sfVector2f A, sfVector2f B, sfVector2f C, sfVector2f D)
{
	sfVector2f vAB;
	sfVector2f vAC;
	sfVector2f vAD;
	sfVector2f vCD;
	sfVector2f vCA;
	sfVector2f vCB;

	float det_ABAC;
	float det_ABAD;
	float det_CDCA;
	float det_CDCB;

	sfBool isCollide;

	vAB.x = B.x - A.x;
	vAB.y = B.y - A.y;

	vAC.x = C.x - A.x;
	vAC.y = C.y - A.y;

	vAD.x = D.x - A.x;
	vAD.y = D.y - A.y;

	vCD.x = D.x - C.x;
	vCD.y = D.y - C.y;

	vCA.x = A.x - C.x;
	vCA.y = A.y - C.y;

	vCB.x = B.x - C.x;
	vCB.y = B.y - C.y;

	det_ABAC = (vAB.x * vAC.y) - (vAB.y * vAC.x);
	det_ABAD = (vAB.x * vAD.y) - (vAB.y * vAD.x);
	det_CDCA = (vCD.x * vCA.y) - (vCD.y * vCA.x);
	det_CDCB = (vCD.x * vCB.y) - (vCD.y * vCB.x);

	isCollide = ((det_ABAC * det_ABAD) < 0) && ((det_CDCA * det_CDCB) < 0);

	return isCollide;
}

sfBool isCollidingPoly(sfVector2f point, MapPrefab poly, sfVector2i posMap)
{
	sfBool result = sfTrue;
	float shiftX = posMap.x * (float)SIZE_TILE;
	float shiftY = posMap.y * (float)SIZE_TILE;

	for (int i = 0; i < poly.nbLine; i++)
	{
		sfVector2f v = { point.x - (poly.line[i].pointA.x + shiftX) ,point.y - (poly.line[i].pointA.y + shiftY) };
		sfVector2f perp = { poly.line[i].dir.y,-poly.line[i].dir.x };
		float dot = Dot(v, perp);
		if (dot > 0)
		{
			result = sfFalse;
		}
	}

	return result;
}

sfBool isCollidingRectRect(sfFloatRect A, sfFloatRect B)
{
	sfBool testL = A.left + A.width > B.left;
	sfBool testR = A.left < B.left + B.width;
	sfBool testU = A.top + A.height > B.top;
	sfBool testD = A.top < B.top + B.height;
	return (testL && testR && testU && testD);
}

// --------------------------------------- == COLLIDING  == ----------------------------------------//

//------------------------------------------------------------------------------------------------------\\

void SetEntityCollidingPos(Entity2D* entity, Circle* circleCollider, Hit hit, sfBool resetVelY)
{
	sfVector2f basePos;
	sfVector2f offset;

	if (resetVelY)
	{
		entity->body.vel.y = 0;
	}

	//player->body.vel.x = 0;
	basePos.x = circleCollider->center.x;
	basePos.y = circleCollider->center.y;

	offset.x = hit.point.x + hit.normal.x * (circleCollider->radius) - basePos.x;
	offset.y = hit.point.y + hit.normal.y * (circleCollider->radius) - basePos.y;

	entity->bodyCollider.topC.center.x += offset.x;
	entity->bodyCollider.topC.center.y += offset.y;

	entity->body.pos.x += offset.x;
	entity->body.pos.y += offset.y;

	int index = 0;
	for (int i = -1; i < 2; i++)
	{
		entity->bodyCollider.bottomPoints[index].x = entity->body.pos.x + (entity->size.x / 2.f)*i;
		entity->bodyCollider.bottomPoints[index].y = entity->body.pos.y;
		index++;
	}
}

Hit CapsCircleCollision(Circle circleCollider, MapPrefab poly, sfVector2i posMap, sfBool groundBlock, CollisionZone zone)
{
	Hit hit;
	hit.isCollided = sfFalse;

	float shiftX = posMap.x * SIZE_TILE;
	float shiftY = posMap.y * SIZE_TILE;

	for (int i = 0; i < poly.nbLine; i++)
	{
		SegmentLine tmpSegment = poly.line[i];
		tmpSegment.pointA.x += shiftX;
		tmpSegment.pointA.y += shiftY;
		tmpSegment.pointB.x += shiftX;
		tmpSegment.pointB.y += shiftY;
		hit = IsCollindingSegmentCircle(tmpSegment, circleCollider);

		if (hit.isCollided)
		{
			sfBool xColliding = (hit.point.x < circleCollider.center.x + circleCollider.radius) && (hit.point.x > circleCollider.center.x - circleCollider.radius);
			sfBool yCollidingTop = hit.point.y < circleCollider.center.y && xColliding;
			sfBool yCollidingBottom = groundBlock && (Vec2fPerp(tmpSegment.dir).y < 0);

			if ((zone == Top  && yCollidingTop)
				|| (tmpSegment.dir.y < 1 && tmpSegment.dir.y > -1 && yCollidingBottom && xColliding))
			{
				return hit;
			}
			else
			{
				hit.isCollided = sfFalse;
			}
		}
	}
	return hit;
}

int GetGroundSegmentID(CollisionMap map, int blockID)
{
	int lineID = -1;

	if (blockID >= 0)
	{
		int nbLine = map.prefabs[blockID].nbLine;
		for (int i = 0; (i < nbLine); i++)
		{
			if (Vec2fPerp(map.prefabs[blockID].line[i].dir).y < 0)
			{
				if (lineID == -1 || Vec2fPerp(map.prefabs[blockID].line[lineID].dir).x == 0)
				{
					lineID = i;
				}
			}
		}
	}

	return lineID;
}

void RecalEntityWithPoint(Entity2D* entity, int pointID)
{
	float offsetX = entity->size.x / 2;

	//recal body
	entity->body.pos.x = entity->bodyCollider.bottomPoints[1].x;
	entity->body.pos.y = entity->bodyCollider.bottomPoints[pointID].y;

	entity->bodyCollider.topC.center.x = entity->body.pos.x;
	entity->bodyCollider.topC.center.y = entity->body.pos.y - entity->size.y + offsetX;

	entity->bodyCollider.rect.top = entity->bodyCollider.topC.center.y;
	entity->bodyCollider.rect.left = entity->body.pos.x - (offsetX);

	int index = 0;
	for (int i = -1; i < 2; i++)
	{
		entity->bodyCollider.bottomPoints[index].x = entity->body.pos.x + (entity->size.x / 2.f)*i;
		entity->bodyCollider.bottomPoints[index].y = entity->body.pos.y;
		index++;
	}

	entity->isOnGround = sfTrue;
}

void DirUpdate(Entity2D* entity, CollisionMap collisionMap, InstanceMaps instanceMaps, int indexMap, sfVector2i blockPos)
{
	entity->blocPos = blockPos;
	entity->blocID = -1;
	int PreviousID = -1;

	if (blockPos.x >= 0 && blockPos.x < instanceMaps.maps[indexMap].width)
	{
		if (blockPos.y >= 0 && blockPos.y < instanceMaps.maps[indexMap].height)
		{
			entity->blocID = instanceMaps.maps[indexMap].colliderID[blockPos.x][blockPos.y];
		}
	}

	if (entity->previousBlocPos.x >= 0 && entity->previousBlocPos.x < instanceMaps.maps[indexMap].width)
	{
		if (entity->previousBlocPos.y >= 0 && entity->previousBlocPos.y < instanceMaps.maps[indexMap].height)
		{
			PreviousID = instanceMaps.maps[indexMap].colliderID[entity->previousBlocPos.x][entity->previousBlocPos.y];
		}
	}

	//RESET OF THE MOVEDIR
	if (entity->blocPos.x != entity->previousBlocPos.x)
	{
		sfVector2f dirLine = { 0,0 };
		sfVector2f A = { 0,0 };
		sfVector2f B = { 0,0 };
		sfVector2f PreA = { 0,0 };
		sfVector2f PreB = { 0,0 };
		float dot;
		if (PreviousID > -1)
		{
			int groundID = GetGroundSegmentID(collisionMap, PreviousID);
			if (Vec2fPerp(collisionMap.prefabs[PreviousID].line[groundID].dir).y < 0)
			{
				PreA = collisionMap.prefabs[PreviousID].line[groundID].pointA;
				PreB = collisionMap.prefabs[PreviousID].line[groundID].pointB;
			}
		}

		if (entity->blocID > -1 && entity->moveDirActive)
		{

			int groundID = GetGroundSegmentID(collisionMap, entity->blocID);
			if (Vec2fPerp(collisionMap.prefabs[entity->blocID].line[groundID].dir).y < 0)
			{
				dirLine = collisionMap.prefabs[entity->blocID].line[groundID].dir;
				A = collisionMap.prefabs[entity->blocID].line[groundID].pointA;
				B = collisionMap.prefabs[entity->blocID].line[groundID].pointB;

			}

			if (PreviousID > -1)
			{
				sfBool testL = (B.y == PreA.y);
				sfBool testR = (A.y == PreB.y);
				sfBool factor = entity->body.vel.x > 0.1 ? testR : testL;

				if (dirLine.x == 1)
				{
					entity->bodyCollider.bottomPoints[1].y = A.y + entity->blocPos.y * SIZE_TILE - (A.x + entity->blocPos.x * SIZE_TILE - entity->bodyCollider.bottomPoints[1].x)* dirLine.y / dirLine.x;
					RecalEntityWithPoint(entity, 1);
				}
			}

		}
		else
		{
			entity->isOnGround = sfFalse;
		}
		entity->previousBlocPos = entity->blocPos;
	}
}

void BodyCollisionTop(Entity2D* entity, CollisionMap collisionMap, InstanceMaps instanceMaps, int indexMap)
{
	sfVector2f actualPoint = entity->bodyCollider.topC.center;
	float radius = entity->bodyCollider.topC.radius;
	actualPoint.y -= radius;

	sfVector2i actualMapBlock = { (int)(actualPoint.x / (float)SIZE_TILE), (int)(actualPoint.y / (float)SIZE_TILE) };

	for (int i = actualMapBlock.x - 1; i < actualMapBlock.x + 2; i++)
	{
		for (int j = actualMapBlock.y - 1; j < actualMapBlock.y + 2; j++)
		{
			if (i >= 0 && i < instanceMaps.maps[indexMap].width)
			{
				if (j >= 0 && j < instanceMaps.maps[indexMap].height)
				{
					int id = instanceMaps.maps[indexMap].colliderID[i][j];
					if (id >= 0)
					{
						sfBool ceilling = sfTrue;
						if (j + 1 < instanceMaps.maps[indexMap].height)
						{
							if (instanceMaps.maps[indexMap].colliderID[i][j + 1] > -1)
							{
								ceilling = sfFalse;
							}
						}
						//printf("Top bloc ID : %d\n", id);

						if (ceilling)
						{
							Hit hit;
							hit = CapsCircleCollision(entity->bodyCollider.topC, collisionMap.prefabs[id], (sfVector2i) { i, j }, sfFalse, Top);

							if (hit.isCollided)
							{
								sfBool resetVelY = entity->body.vel.y < 0.1f;
								SetEntityCollidingPos(entity, &entity->bodyCollider.topC, hit, resetVelY);
							}
						}
					}
				}
			}
		}
	}
}

Hit BodyCollisionBottom(Entity2D* entity, CollisionMap collisionMap, InstanceMaps instanceMaps, int indexMap, int pointID)
{
	Hit hit;
	hit.isCollided = sfFalse;

	sfVector2f currentPoint = entity->bodyCollider.bottomPoints[pointID];
	sfVector2i actualMapBlock = { (int)(currentPoint.x / (float)SIZE_TILE), (int)(currentPoint.y / (float)SIZE_TILE) };

	if (currentPoint.x < 0.f || currentPoint.y < 0.f)
	{
		actualMapBlock.x = -1;
		actualMapBlock.y = -1;
	}

	if (pointID == 1)
	{
		entity->blocPos = actualMapBlock;
	}


	if (actualMapBlock.x >= 0 && actualMapBlock.x < instanceMaps.maps[indexMap].width)
	{
		if (actualMapBlock.y >= 0 && actualMapBlock.y < instanceMaps.maps[indexMap].height)
		{
			int id = instanceMaps.maps[indexMap].colliderID[actualMapBlock.x][actualMapBlock.y];

			if (pointID == 1)
			{
				int actGroundID = GetGroundSegmentID(collisionMap, id);
				int preGroundID = GetGroundSegmentID(collisionMap, entity->blocID);

				if (id == -1 || entity->blocID == -1)
				{
					entity->moveDirActive = sfFalse;
				}
				else if (collisionMap.prefabs[id].line[actGroundID].dir.y != 0 && collisionMap.prefabs[entity->blocID].line[preGroundID].dir.y == 0 && entity->isOnGround)
				{
					entity->moveDirActive = sfTrue;
				}
				else if (collisionMap.prefabs[id].line[actGroundID].dir.y == 0 && collisionMap.prefabs[entity->blocID].line[preGroundID].dir.y != 0 && entity->isOnGround)
				{
					entity->needDirUpdate = sfTrue;
					DirUpdate(entity, collisionMap, instanceMaps, indexMap, entity->blocPos);
					entity->moveDirActive = sfFalse;
				}
			}

			if (id >= 0)
			{
				if (pointID == 1)
				{
					entity->blocID = id;
				}

				int groundID = GetGroundSegmentID(collisionMap, id);

				if (actualMapBlock.y - 1 >= 0 && instanceMaps.maps[indexMap].colliderID[actualMapBlock.x][actualMapBlock.y - 1] >= 0)
				{
					if (collisionMap.prefabs[id].line[groundID].dir.y == 0.0f)
					{
						hit.isCollided = sfFalse;
						return hit;
					}
				}

				if (groundID >= 0)
				{
					if (collisionMap.prefabs[id].line[groundID].dir.y != 0.0f)
					{
						if (pointID == 1)
						{
							// function de collision point segment 
							sfVector2f pointA = { collisionMap.prefabs[id].line[groundID].pointA.x + actualMapBlock.x * SIZE_TILE , collisionMap.prefabs[id].line[groundID].pointA.y + actualMapBlock.y * SIZE_TILE };
							sfVector2f v = { currentPoint.x - pointA.x  , currentPoint.y - pointA.y };
							sfVector2f perp = Vec2fPerp(collisionMap.prefabs[id].line[groundID].dir);
							float dot = Dot(v, perp);

							if (dot <= 0)
							{
								hit.point.x = currentPoint.x;
								hit.point.y = currentPoint.y + dot;
								hit.normal = perp;
								hit.isCollided = sfTrue;
								entity->moveDirActive = sfTrue;
							}

							return hit;

						}
					}
					else
					{
						// function de collision point segment 
						sfVector2f pointA = { collisionMap.prefabs[id].line[groundID].pointA.x + actualMapBlock.x * SIZE_TILE , collisionMap.prefabs[id].line[groundID].pointA.y + actualMapBlock.y * SIZE_TILE };
						sfVector2f v = { currentPoint.x - pointA.x ,  currentPoint.y - pointA.y };
						sfVector2f perp = Vec2fPerp(collisionMap.prefabs[id].line[groundID].dir);
						float dot = Dot(v, perp);

						//printf("dot %f  bloc ID : %d\n", dot, id);

						if (dot <= 0)
						{
							hit.point.x = currentPoint.x;
							hit.point.y = currentPoint.y + dot;
							hit.normal = perp;
							hit.isCollided = sfTrue;
						}

						return hit;
					}
				}
			}
		}
	}

	return hit;
}

sfBool IsCollindingPointRect(sfVector2f point, sfFloatRect rect)
{
	sfBool testLeft = (point.x >= rect.left);
	sfBool testRight = (point.x <= rect.left + rect.width - 1);
	sfBool testUp = (point.y >= rect.top);
	sfBool testDown = (point.y <= rect.top + rect.height);

	return (testLeft && testRight && testUp && testDown);
}

sfVector2f BodyCollisionSide(BodyCollider* collider, CollisionMap collisionMap, InstanceMaps instanceMaps, int indexMap, int dirX)
{
	sfVector2f actualPoint = collider->topC.center;
	float radius = collider->topC.radius;
	actualPoint.y -= radius;
	sfVector2i actualMapBlock = { (int)(actualPoint.x / (float)SIZE_TILE), (int)(actualPoint.y / (float)SIZE_TILE) };

	for (int i = actualMapBlock.x - 2; i < actualMapBlock.x + 3; i++)
	{
		for (int j = actualMapBlock.y - 2; j < actualMapBlock.y + 3; j++)
		{
			if (i >= 0 && i < instanceMaps.maps[indexMap].width)
			{
				if (j >= 0 && j < instanceMaps.maps[indexMap].height)
				{
					int id = instanceMaps.maps[indexMap].colliderID[i][j];

					if (id >= 0)
					{
						int groundID = GetGroundSegmentID(collisionMap, id);
						sfBool isWallBlock = sfFalse;

						//printf("dir X : %d\n", dirX);

						if (i == instanceMaps.maps[indexMap].width - 1 || i == 0)
						{
							isWallBlock = sfTrue;
						}
						else if (i < instanceMaps.maps[indexMap].width - 1 && i > 0)
						{
							if (instanceMaps.maps[indexMap].colliderID[i - dirX][j] == -1)
							{
								isWallBlock = sfTrue;
							}
						}

						if (collisionMap.prefabs[id].line[groundID].dir.y != 0 || !isWallBlock)
						{
							continue;
							//(sfVector2f) { -1.f, -1.f };
						}

						for (int k = 0; k < collisionMap.prefabs[id].nbLine; k++)
						{
							sfBool isCollide = sfFalse;
							sfVector2f pointA_pos = { 0 };
							sfVector2f pointB_pos = { 0 };
							sfVector2f pointCenter_pos = { 0 };

							pointA_pos.x = collisionMap.prefabs[id].line[k].pointA.x + (i * (float)SIZE_TILE);
							pointA_pos.y = collisionMap.prefabs[id].line[k].pointA.y + (j * (float)SIZE_TILE);


							isCollide = IsCollindingPointRect(pointA_pos, collider->rect);

							if (isCollide)
							{
								//						printf("A pos : %f %f\n", pointA_pos.x, pointA_pos.y);
								return pointA_pos;
							}

							pointB_pos.x = collisionMap.prefabs[id].line[k].pointB.x + (i * (float)SIZE_TILE);
							pointB_pos.y = collisionMap.prefabs[id].line[k].pointB.y + (j * (float)SIZE_TILE);

							isCollide = IsCollindingPointRect(pointB_pos, collider->rect);

							if (isCollide)
							{
								//							printf("B pos : %f %f\n", pointB_pos.x, pointB_pos.y);
								return pointB_pos;
							}

							pointCenter_pos.x = (pointA_pos.x + pointB_pos.x) / 2;
							pointCenter_pos.y = (pointA_pos.y + pointB_pos.y) / 2;

							isCollide = IsCollindingPointRect(pointCenter_pos, collider->rect);

							if (isCollide)
							{
								//								printf("Center pos : %f %f\n", pointCenter_pos.x, pointCenter_pos.y);
								return pointCenter_pos;
							}
						}
					}
				}
			}
		}
	}
	//	printf("NULL\n");
	return (sfVector2f) { -1.f, -1.f };
}

void CalculMoveDir(Entity2D* entity, CollisionMap collisionMap, InstanceMaps instanceMaps, int indexMap)
{
	int indexSegmentGround = 0;
	if (entity->blocPos.x >= 0 && entity->blocPos.x < instanceMaps.maps[indexMap].width)
	{
		if (entity->blocPos.y >= 0 && entity->blocPos.y < instanceMaps.maps[indexMap].height)
		{
			int mapID = instanceMaps.maps[indexMap].colliderID[entity->blocPos.x][entity->blocPos.y];
			int nbLine = collisionMap.prefabs[mapID].nbLine;
			if (mapID > -1)
			{
				int groundID = GetGroundSegmentID(collisionMap, mapID);

				if (collisionMap.prefabs[mapID].line[groundID].dir.y != 0.0f)
				{
					entity->moveDir = collisionMap.prefabs[mapID].line[groundID].dir;
				}
				else
				{
					entity->moveDirActive = sfFalse;
				}
			}
		}
	}
}

void Entity2DMapCollision(Entity2D* entity, CollisionMap collisionMap, InstanceMaps instanceMaps, int indexMap)
{
	Hit hit;
	sfVector2f hitPoint;
	// Side collision
	int dir = entity->dir;
	if (entity->body.vel.x < -0.1f)
	{
		dir = -1;
	}
	else if (entity->body.vel.x > 0.1f)
	{
		dir = 1;
	}


	int mapID = 0;
	if (entity->blocPos.x >= 0 && entity->blocPos.x < instanceMaps.maps[indexMap].width)
	{
		if (entity->blocPos.y >= 0 && entity->blocPos.y < instanceMaps.maps[indexMap].height)
		{
			mapID = instanceMaps.maps[indexMap].colliderID[entity->blocPos.x][entity->blocPos.y];
		}
	}
	int nbLine = collisionMap.prefabs[mapID].nbLine;
	int groundID = -1;
	if (mapID > -1)
	{
		groundID = GetGroundSegmentID(collisionMap, mapID);
	}
	if ((groundID > -1 && collisionMap.prefabs[mapID].line[groundID].dir.y == 0.0f) || mapID == -1)
	{
		hitPoint = BodyCollisionSide(&entity->bodyCollider, collisionMap, instanceMaps, indexMap, dir);

		if (hitPoint.x > -1)
		{
			//printf("OK %f %f\n", hitPoint.x, hitPoint.y);

			entity->hasHitWall = sfTrue;
			if (entity->body.vel.x > 0.1f)
			{
				float pointDistanceX = (hitPoint.x) - (entity->bodyCollider.rect.left + entity->bodyCollider.rect.width);
				entity->body.pos.x += pointDistanceX;
				UpdateEntity2DColl(entity);
				entity->body.vel.x = 0.0f;
			}
			else if (entity->body.vel.x < -0.1f)
			{
				float pointDistanceX = (hitPoint.x) - entity->bodyCollider.rect.left;
				entity->body.pos.x += pointDistanceX;
				UpdateEntity2DColl(entity);
				entity->body.vel.x = 0.0f;
			}
		}
		else
		{
			entity->hasHitWall = sfFalse;
		}
	}
	// Bottom collision

	entity->needDirUpdate = sfFalse;

	hit = BodyCollisionBottom(entity, collisionMap, instanceMaps, indexMap, 1);

	if (entity->moveDirActive)
	{
		CalculMoveDir(entity, collisionMap, instanceMaps, indexMap);
		DirUpdate(entity, collisionMap, instanceMaps, indexMap, entity->blocPos);
	}
	if (entity->needDirUpdate)
	{
		DirUpdate(entity, collisionMap, instanceMaps, indexMap, entity->blocPos);
	}

	if (entity->body.vel.y > 0.0f || entity->isOnGround)
	{
		if (hit.isCollided)
		{
			// RECALAGE
			entity->bodyCollider.bottomPoints[1] = hit.point;
			entity->body.vel.y = 0.f;
			entity->isOnGround = sfTrue;
			RecalEntityWithPoint(entity, 1);
		}
		else if (!entity->moveDirActive)
		{
			entity->isOnGround = sfFalse;
			for (int i = 0; i < 3; i += 2)
			{
				hit = BodyCollisionBottom(entity, collisionMap, instanceMaps, indexMap, i);
				if (hit.isCollided)
				{
					entity->bodyCollider.bottomPoints[i] = hit.point;
					RecalEntityWithPoint(entity, i);
					entity->body.vel.y = 0.f;
					entity->isOnGround = sfTrue;
					entity->moveDirActive = sfFalse;
				}
			}
		}
	}


	//printf("isOnGround : %d\n", entity->isOnGround);
	entity->body.Enablegravity = !entity->isOnGround;

	//if (entity->isOnGround)
	//{
	//	entity->hasToCheckGround = sfFalse;
	//	CalculMoveDir(entity, collisionMap, instanceMaps, indexMap);
	//}


	// Top collision
	BodyCollisionTop(entity, collisionMap, instanceMaps, indexMap);

	if (entity->isOnGround && entity->hasHitWall)
	{
		entity->moveDirActive = sfFalse;
		entity->body.vel.y = 0.0f;
	}
}
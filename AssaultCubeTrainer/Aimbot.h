#pragma once
#include <Windows.h>
#include "Offsets.h"

namespace Aimbot
{
	void DoAimbot(DWORD moduleBaseAddress, HANDLE processHandle);
	float CalcAngleBetweenVectors(Entity::vector3D vector1, Entity::vector3D vector2);
	Entity::vector3D ViewToVec(Entity::eulerAngles viewAngles);
	Entity::eulerAngles VecToView(Entity::vector3D aimVector);
	Entity::vector3D VecSubtraction(Entity::vector3D vector1, Entity::vector3D vector2);
	float RadiansToDegrees(float rad);
	float DegreesToRadians(float deg);
	float VectorDotProduct(Entity::vector3D vector1, Entity::vector3D vector2);
	float CalcVectorLength(Entity::vector3D vector);
}

//007A1C48
//00797EBC + 0
//00797EA8 + 14
//007A1B08 + 374
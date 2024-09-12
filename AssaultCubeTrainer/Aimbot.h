#pragma once
#include <Windows.h>
#include "Offsets.h"

namespace Aimbot
{
	void DoAimbot(DWORD moduleBaseAddress, HANDLE processHandle);
	Entity::eulerAngles calcAngle(Entity::vector3D player, Entity::vector3D enemy);
	float radiansToDegrees(float rad);
}
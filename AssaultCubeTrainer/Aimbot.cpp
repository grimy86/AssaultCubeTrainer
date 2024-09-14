#include <math.h>
#include "Aimbot.h"

namespace Aimbot
{
	void DoAimbot(DWORD moduleBaseAddress, HANDLE processHandle)
	{
		if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) // most significant bit for 2-byte value
		{
			DWORD localPlayerPtr{};
			if (!ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(moduleBaseAddress + Offsets::localPlayerPtr4), &localPlayerPtr, sizeof(localPlayerPtr), nullptr) || !localPlayerPtr)
			{
				return;
			}
			int health{};
			if (!ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(localPlayerPtr + Offsets::healthOffset), &health, sizeof(health), nullptr) || health <= 0)
			{
				return;
			}
			DWORD entityListPtr{};
			if (!ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(moduleBaseAddress + Offsets::entityList), &entityListPtr, sizeof(entityListPtr), nullptr) || !entityListPtr)
			{
				return;
			}
			DWORD entityAddress{};
			if (!ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(entityListPtr + 0x0C), &entityAddress, sizeof(entityAddress), nullptr) || !entityAddress)
			{
				return;
			}
			int entityHealth{};
			if (!ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(entityAddress + Offsets::healthOffset), &entityHealth, sizeof(entityHealth), nullptr) || entityHealth <= 0)
			{
				return;
			}
			Entity::vector3D playerPosition{};
			if (!ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(localPlayerPtr + Offsets::camVector), &playerPosition, sizeof(playerPosition), nullptr))
			{
				return;
			}
			Entity::vector3D entityPosition{};
			if (!ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(entityAddress + Offsets::camVector), &entityPosition, sizeof(entityPosition), nullptr))
			{
				return;
			}

			Entity::eulerAngles aimAngles{ calcAngle(playerPosition, entityPosition) };
			WriteProcessMemory(processHandle, reinterpret_cast<LPVOID>(localPlayerPtr + Offsets::viewAngles), &aimAngles, sizeof(aimAngles), nullptr);
		}
	}

	Entity::eulerAngles calcAngle(Entity::vector3D player, Entity::vector3D enemy)
	{
		//Get "grey" vector = destination - source
		Entity::vector3D aimVector
		{
			enemy.x - player.x, //distance between x postions
			enemy.y - player.y, //distance between y positions
			enemy.z - player.z //distance between z positions
		};

		//Yaw angles
		Entity::eulerAngles aimAngles{};
		if (aimVector.x >= 0.0f && aimVector.y < 0.0f) // +x, -y
		{
			aimAngles.Yaw = radiansToDegrees(atanf(aimVector.x / -aimVector.y));
		}
		else if (aimVector.x >= 0.0f && aimVector.y >= 0.0f) // +x, +y
		{
			aimAngles.Yaw = radiansToDegrees(atanf(aimVector.y /aimVector.x)) + 90.0f;
		}
		else if (aimVector.x < 0.0f && aimVector.y >= 0.0f) // -x, +y
		{
			aimAngles.Yaw = radiansToDegrees(atanf(-aimVector.x / aimVector.y)) + 180.0f;
		}
		else // -x, -y
		{
			aimAngles.Yaw = radiansToDegrees(atanf(-aimVector.y / -aimVector.x)) + 270.0f;
		}
		
		//Pitch angles
		if (aimVector.z >= 0.0f)
		{
			aimAngles.Pitch = radiansToDegrees(atanf(aimVector.z / sqrtf(aimVector.x * aimVector.x + aimVector.y * aimVector.y)));
		}
		else
		{
			aimAngles.Pitch = -radiansToDegrees(atanf(-aimVector.z / sqrtf(aimVector.x * aimVector.x + aimVector.y * aimVector.y)));
		}
		return aimAngles;
	}

	float radiansToDegrees(float rad)
	{
		return ((rad * 180.0f) / 3.14159265359f);
	}
}
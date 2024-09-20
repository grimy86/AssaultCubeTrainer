#include <math.h>
#include "Aimbot.h"
#include "ExtProc.h"

namespace Aimbot
{
	void DoAimbot(ExtProc& extProc)
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

#pragma region 3D
	float CalcAngleBetweenVectors(Entity::vector3D vector1, Entity::vector3D vector2)
	{
		return RadiansToDegrees(acos((VectorDotProduct(vector1, vector2)) / (CalcVectorLength(vector1) * CalcVectorLength(vector2))));
	}

	Entity::vector3D ViewToVec(Entity::eulerAngles viewAngles)
	{
		Entity::vector3D viewVector{};
		if (viewAngles.Yaw >= 0.0f && viewAngles.Yaw < 90.0f) // +x, -y
		{
			viewVector.y = -1.0f;
			viewVector.x = tan(DegreesToRadians(viewAngles.Yaw));

		}
		else if (viewAngles.Yaw >= 90.0f && viewAngles.Yaw < 180.0f) // +x, +y
		{
			viewVector.x = 1.0f;
			viewVector.y = tan(DegreesToRadians(viewAngles.Yaw - 90.0f));
		}
		else if (viewAngles.Yaw >= 180.0f && viewAngles.Yaw < 270.0f) // -x, +y
		{
			viewVector.y = 1.0f;
			viewVector.x = -tan(DegreesToRadians(viewAngles.Yaw - 180.0f));
		}
		else // -x, -y
		{
			viewVector.x = -1.0f;
			viewVector.y = -tan(DegreesToRadians(viewAngles.Yaw - 270.0f));
		}

		//Pitch angles
		if (viewAngles.Pitch >= 0.0f)
		{
			viewVector.z = tan(DegreesToRadians(viewAngles.Pitch)) * sqrt((viewVector.x * viewVector.x) + (viewVector.y * viewVector.y));
		}
		else
		{
			viewVector.z = -tan(DegreesToRadians(viewAngles.Pitch)) * sqrt((viewVector.x * viewVector.x) + (viewVector.y * viewVector.y));
		}
		return viewVector;
	}

	Entity::eulerAngles VecToView(Entity::vector3D aimVector)
	{
		//Yaw angles
		Entity::eulerAngles aimAngles{};
		if (aimVector.x >= 0.0f && aimVector.y < 0.0f) // +x, -y
		{
			aimAngles.Yaw = RadiansToDegrees(atanf(aimVector.x / -aimVector.y));
		}
		else if (aimVector.x >= 0.0f && aimVector.y >= 0.0f) // +x, +y
		{
			aimAngles.Yaw = RadiansToDegrees(atanf(aimVector.y / aimVector.x)) + 90.0f;
		}
		else if (aimVector.x < 0.0f && aimVector.y >= 0.0f) // -x, +y
		{
			aimAngles.Yaw = RadiansToDegrees(atanf(-aimVector.x / aimVector.y)) + 180.0f;
		}
		else // -x, -y
		{
			aimAngles.Yaw = RadiansToDegrees(atanf(-aimVector.y / -aimVector.x)) + 270.0f;
		}

		//Pitch angles
		if (aimVector.z >= 0.0f)
		{
			aimAngles.Pitch = RadiansToDegrees(atanf(aimVector.z / sqrtf(aimVector.x * aimVector.x + aimVector.y * aimVector.y)));
		}
		else
		{
			aimAngles.Pitch = -RadiansToDegrees(atanf(-aimVector.z / sqrtf(aimVector.x * aimVector.x + aimVector.y * aimVector.y)));
		}
		return aimAngles;
	}

	Entity::vector3D VecSubtraction(Entity::vector3D vector1, Entity::vector3D vector2)
	{
		//Get "grey" vector = destination - source
		Entity::vector3D vectorBetweenPoints //vector to enemy (dest(enemy) - source(player))
		{
			vector1.x - vector2.x, //distance between x postions
			vector1.y - vector2.y, //distance between y positions
			vector1.z - vector2.z //distance between z positions
		};
	}

	float RadiansToDegrees(float rad)
	{
		return ((rad * 180.0f) / 3.14159265359f);
	}

	float DegreesToRadians(float deg)
	{
		return ((deg / 180.0f) * 3.14159265359f);
	}

	float VectorDotProduct(Entity::vector3D vector1, Entity::vector3D vector2)
	{
		return (vector1.x * vector2.x) + (vector1.y * vector2.y) + (vector1.z * vector2.z);
	}

	float CalcVectorLength(Entity::vector3D vector)
	{
		return sqrt((vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z));
	}
#pragma endregion


	

	bool IsTeamGame(const DWORD& gameMode)
	{
		if (!gameMode)
		{
			return;
		}

		if (gameMode == 0 || gameMode == 4 || gameMode == 5 || gameMode == 7 || gameMode == 11 || gameMode == 13 ||
			gameMode == 14 || gameMode == 16 || gameMode == 17 || gameMode == 20 || gameMode == 21) return true;
		else return false;
	}
}
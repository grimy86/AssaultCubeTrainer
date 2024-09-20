#include "Aimbot.h"
#include <math.h>
#include <iostream>

namespace Aimbot
{
	void DoAimbot(ExtProc& extProc, DWORD moduleBaseAddress)
	{
		if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) // most significant bit for 2-byte value
		{
			DWORD localPlayerPtr{ extProc.RPM<DWORD>(moduleBaseAddress + Offsets::localPlayerPtr4) };

			if (!localPlayerPtr)
			{
				return;
			}

			int health{ extProc.RPM<int>(localPlayerPtr + Offsets::healthOffset) };
			if (health <= 0)
			{
				return;
			}

			DWORD entityListPtr{ extProc.RPM<DWORD>(moduleBaseAddress + Offsets::entityList) };
			if (!entityListPtr)
			{
				return;
			}

			DWORD entityAddress{ extProc.RPM<DWORD>(entityListPtr + 0x04) };
			if (!entityAddress)
			{
				return;
			}

			int entityHealth{ extProc.RPM<int>(entityAddress + Offsets::healthOffset) };
			if (entityHealth <= 0)
			{
				return;
			}

			Entity::vector3D playerPosition{ extProc.RPM<Entity::vector3D>(localPlayerPtr + Offsets::camVector) };
			Entity::vector3D entityPosition{ extProc.RPM<Entity::vector3D>(entityAddress + Offsets::camVector) };
			Entity::vector3D aimVector{ VecSubtraction(entityPosition, playerPosition) }; //destination - source
			Entity::eulerAngles playerViewAngles{ extProc.RPM<Entity::eulerAngles>(localPlayerPtr + Offsets::viewAngles) };
			Entity::vector3D viewVector{ ViewToVec(playerViewAngles) };
			float fieldOfView{ CalcAngleBetweenVectors(aimVector, viewVector) };
			std::cout << "FOV:" << fieldOfView << '\n';

			if (fieldOfView < 5.0f)
			{
				extProc.WPM<Entity::eulerAngles>(localPlayerPtr + Offsets::viewAngles, VecToView(aimVector));
			}
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

		return vectorBetweenPoints;
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

	/*bool IsTeamGame(const DWORD& gameMode)
	{
		if (!gameMode)
		{
			return;
		}

		if (gameMode == 0 || gameMode == 4 || gameMode == 5 || gameMode == 7 || gameMode == 11 || gameMode == 13 ||
			gameMode == 14 || gameMode == 16 || gameMode == 17 || gameMode == 20 || gameMode == 21) return true;
		else return false;
	}
	*/
}
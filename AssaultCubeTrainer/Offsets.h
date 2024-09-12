#pragma once
#include <Windows.h>

namespace Offsets
{
	inline const char* processName { "ac_client.exe" };
	inline const char* moduleName { "ac_client.exe" };

	//DWORD = 4bytes
	const DWORD localPlayerPtr1 { 0x17E0A8 };
	const DWORD localPlayerPtr2 { 0x17E254 };
	const DWORD localPlayerPtr3 { 0x17E360 };
	const DWORD localPlayerPtr4 { 0x18AC00 };
	const DWORD localPlayerPtr5 { 0x195404 };

	//Health
	const DWORD healthOffset { 0xEC };

	//Campos
	const DWORD camVector { 0x04 };

	//Feetpos
	const DWORD feetVector{ 0x28 };

	//Viewangles
	const DWORD viewAngles{ 0x34 };

	//Entity teams
	const DWORD teamcheck { 0x30C };

	//Points to array of entity pointers[0] (0x191FCC)
	const DWORD entityList{ 0x18AC04 };
}

namespace Entity
{
	struct vector3D //for both cam and feetpos
	{
		float x {};
		float y {};
		float z {};
	};

	struct eulerAngles //for both cam and feetpos
	{
		float Yaw {};
		float Pitch {};
	};
}
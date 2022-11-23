#pragma once
#include <vector>
#include <Windows.h>


namespace hacks {
	DWORD GetAddressFromSignature(std::vector<int> signature, DWORD startaddress = 0, DWORD endaddress = 0);
	DWORD GetLocalPlayer(std::vector<int> playerSig);

	struct TerrariaSignatures {
		std::vector<int> TimeInstruction = { 0xF4, 0x00, 0x74, 0x02, 0x33, 0xF6, 0x89, 0x35, -1, -1, -1, -1 };
		std::vector<int> LocalPlayer = { 0xA1,  -1, -1, -1, -1, 0x8B, 0x15, -1, -1, -1, -1, 0x3B, 0x50, 0x04, 0x73, 0x05, 0x8B, 0x44, 0x90, 0x08, 0xC3, 0xE8 };
		std::vector<int> ScreenInstruction = { 0xA1, -1, -1, -1, -1, -1, -1, -1, -1, 0x83, 0xD0, -1, 0x89, 0x85, -1, -1, -1, -1, 0xDB, 0x85, -1, -1, -1, -1, 0xD9, 0x9D, -1, -1, -1, -1, 0xD9, 0x85, -1, -1, -1, -1, 0xD9,0x9D, -1, -1, -1, -1, 0xD9, 0x85, -1, -1, -1, -1, 0xD8, 0x85, -1, -1, -1, -1, 0xD8, 0x35, -1, -1, -1, -1, 0xD9, 0x9D };
		std::vector<int> NPCArray = { 0x8B, 0x4C, -1, -1, 0x80, 0x79, 0x20, -1, 0x0F, 0x84, -1, -1, -1, -1, 0x8B, 0x91 };
		std::vector<int> fNewNPC = { 0xFF, 0x50, -1, 0x85, 0xC0, 0x74, -1, 0x83, 0x7D, 0x20 };
	};

	struct PlayerOffsets {
		DWORD PositionX = 0x28;
		DWORD PositionY = 0x2C;
		DWORD VelocityX = 0x30;
		DWORD VelocityY = 0x34;
		DWORD Width = 0x18;
		DWORD Height = 0x1C;
		DWORD NamePtr = 0x8C;
		DWORD ArmorPtr = 0xB4;
		DWORD InventoryPtr = 0xD8;
		DWORD Health = 0x404;
		DWORD MaxHealth = 0x3FC;
	};

	struct InventoryOffsets {
		DWORD vTable = 0x0;
		DWORD NumberOfElements = 0x4;
		DWORD itemSlot1 = 0x8;
		DWORD itemSlot2 = 0xC;
		DWORD itemSlot3 = 0x10;
		DWORD itemSlot4 = 0x14;

	};

	struct ItemOffsets {
		DWORD vTable = 0x0;
		DWORD active = 0x20;
		DWORD type = 0xA0;
		DWORD useAnimation = 0xAC;
		DWORD useTime = 0xB0;
		DWORD stack = 0xB4;
		DWORD maxStack = 0xB8;
		DWORD pick = 0xBC;
		DWORD axe = 0xC0;
		DWORD hammer = 0xC4;
		DWORD damage = 0xD8;
		DWORD knockback = 0xDC;
		DWORD stringColor = 0x100;
		DWORD shoot = 0x10C;
		DWORD shootSpeed = 0x110;
		DWORD value = 0x12C;
		DWORD netID = 0x140;


	};

	struct NpcOffsets {
		DWORD width = 0x18;
		DWORD height = 0x1C;
		DWORD active = 0x20;
		DWORD PosX = 0x38;
		DWORD PosY = 0x3C;
		DWORD _givenNamePtr = 0x60;
		DWORD type = 0xE8;
		DWORD aiStyle = 0xF0;
		DWORD Health = 0x10C;
		DWORD HealthMax = 0x110;
		DWORD townNPC = 0x1b0;
		DWORD friendly = 0x1b4;
		


	};

	struct _givenName {
		DWORD vTable = 0x0;
		DWORD m_stringLength = 0x4;
		DWORD m_firstChar = 0x8;

	};
}

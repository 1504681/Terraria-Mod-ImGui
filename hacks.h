#pragma once
#include <vector>
#include <Windows.h>


namespace hacks {
	DWORD GetAddressFromSignature(std::vector<int> signature, DWORD startaddress = 0, DWORD endaddress = 0);
	DWORD GetLocalPlayer(std::vector<int> playerSig);

	struct TerrariaSignatures {
		std::vector<int> TimeInstruction = { 0xF4, 0x00, 0x74, 0x02, 0x33, 0xF6, 0x89, 0x35, -1, -1, -1, -1 };
		std::vector<int> LocalPlayer = { 0xA1,  -1, -1, -1, -1, 0x8B, 0x15, -1, -1, -1, -1, 0x3B, 0x50, 0x04, 0x73, 0x05, 0x8B, 0x44, 0x90, 0x08, 0xC3, 0xE8 };
	};

	struct PlayerOffsets {
		DWORD PositionX = 0x28;
		DWORD PositionY = 0x2C;
		DWORD VelocityX = 0x30;
		DWORD VelocityY = 0x34;
		DWORD NamePtr = 0x84;
		DWORD ArmorPtr = 0xAC;
		DWORD InventoryPtr = 0xD0;
		DWORD BankPtr = 0xDC;
		DWORD Health = 0x3E4;
		DWORD MaxHealth = 0x3DC;


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
		DWORD type = 0x9C;
		DWORD useAnimation = 0xA8;
		DWORD useTime = 0xAC;
		DWORD stack = 0xB0;
		DWORD maxStack = 0xB4;
		DWORD pick = 0xB8;
		DWORD axe = 0xBC;
		DWORD hammer = 0xC0;
		DWORD damage = 0xD4;
		DWORD knockback = 0xD8;
		DWORD stringColor = 0xFC;
		DWORD shoot = 0x108;
		DWORD shootSpeed = 0x10C;
		DWORD value = 0x128;
		DWORD netID = 0x13C;


	};
}

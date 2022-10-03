#include "hacks.h"
#include <iostream>

DWORD hacks::GetAddressFromSignature(std::vector<int> signature, DWORD startaddress, DWORD endaddress) {
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	if (startaddress == 0) {
		startaddress = (DWORD)(si.lpMinimumApplicationAddress);
	}
	if (endaddress == 0) {
		endaddress = (DWORD)(si.lpMaximumApplicationAddress);
	}

	MEMORY_BASIC_INFORMATION mbi{ 0 };
	DWORD protectflags = (PAGE_GUARD | PAGE_NOCACHE | PAGE_NOACCESS);

	for (DWORD i = startaddress; i < endaddress - signature.size(); i++) {
		//std::cout << "scanning: " << std::hex << i << std::endl;
		if (VirtualQuery((LPCVOID)i, &mbi, sizeof(mbi))) {
			if (mbi.Protect & protectflags || !(mbi.State & MEM_COMMIT)) {
				//std::cout << "Bad Region! Region Base Address: " << mbi.BaseAddress << " | Region end address: " << std::hex << (int)((DWORD)mbi.BaseAddress + mbi.RegionSize) << std::endl;
				i += mbi.RegionSize;
				continue; // if bad adress then dont read from it
			}
			//std::cout << "Good Region! Region Base Address: " << mbi.BaseAddress << " | Region end address: " << std::hex << (int)((DWORD)mbi.BaseAddress + mbi.RegionSize) << std::endl;
			for (DWORD k = (DWORD)mbi.BaseAddress; k < (DWORD)mbi.BaseAddress + mbi.RegionSize - signature.size(); k++) {
				for (DWORD j = 0; j < signature.size(); j++) {
					if (signature.at(j) != -1 && signature.at(j) != *(byte*)(k + j))
						break;
					if (j + 1 == signature.size())
						return k;
				}
			}
			i = (DWORD)mbi.BaseAddress + mbi.RegionSize;
		}
	}
	return NULL;
}

DWORD hacks::GetLocalPlayer(std::vector<int> playerSig) {
	DWORD SigStart = GetAddressFromSignature(playerSig, 0x10000000, 0x70000000);

	if (SigStart != NULL) {
		std::cout << "Player Signature Match... Calculating Address..." << std::endl;
		DWORD eax = *(DWORD*)(*(DWORD*)(SigStart + 0x01));
		DWORD edx = *(DWORD*)(*(DWORD*)(SigStart + 0x07));
		return *(DWORD*)(eax + edx * 4 + 0x08);
	}
	else {
		std::cout << "Could not find local player!" << std::endl;
		return NULL;
	}

}

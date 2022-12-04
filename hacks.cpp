#include "hacks.h"
#include <iostream>


DWORD hacks::GetAddressFromSignature(std::vector<int> signature, DWORD startaddress, DWORD endaddress) {
    // Get the system information
    SYSTEM_INFO si;
    GetSystemInfo(&si);

    // If no startaddress was specified, use the minimum application address
    if (startaddress == 0) {
        startaddress = (DWORD)(si.lpMinimumApplicationAddress);
    }
    // If no endaddress was specified, use the maximum application address
    if (endaddress == 0) {
        endaddress = (DWORD)(si.lpMaximumApplicationAddress);
    }

    // Initialize the bad character skip array
    std::vector<int> bad_char_skip(256, signature.size());
    for (size_t i = 0; i < signature.size(); i++) {
        // If the character at index i is not a wildcard (-1), set the skip distance
        // for that character to the length of the signature minus i minus 1
        if (signature[i] != -1) {
            bad_char_skip[signature[i]] = signature.size() - i - 1;
        }
    }

    // Initialize the good suffix skip array
    std::vector<int> good_suffix_skip(signature.size() + 1);
    std::vector<int> suffixes(signature.size() + 1);
    for (int i = signature.size(); i >= 0; i--) {
        if (i == signature.size()) {
            // If i is at the end of the signature, set the skip distance to 1
            good_suffix_skip[i] = 1;
        }
        else {
            // Initialize the skip distance to the length of the signature
            good_suffix_skip[i] = signature.size();
            for (int j = i + 1; j < signature.size(); j++) {
                // If the character at index i - 1 is the same as the character at
                // index j - 1 and the suffix at index j is not a wildcard (-1),
                // set the skip distance to the length of the signature minus j
                if (signature[i - 1] == signature[j - 1] && suffixes[j] != -1) {
                    good_suffix_skip[i] = signature.size() - j;
                    suffixes[i] = j;
                    break;
                }
            }
        }
    }

    // Set i to 0, which will be the starting index for the search
    int i = 0;
    // Continue searching while i is less than or equal to the difference between the endaddress
    // and the startaddress minus the length of the signature
    while (i <= (int)(endaddress - startaddress) - (int)signature.size()) {
        // Set j to the index of the last character in the signature
        int j = signature.size() - 1;
        // Continue moving backwards through the signature and the memory range
        // until a mismatch is found or j is less than 0
        while (j >= 0 && (signature[j] == -1 || signature[j] == *(byte*)(startaddress + i + j))) {
            j--;
        }
        // If j is less than 0, the signature has been found, so return the address
        // at which it was found (startaddress + i)
        if (j < 0) {
            return startaddress + i;
        }
        // Otherwise, check whether the good suffix skip distance is greater than
        // the bad character skip distance for the character at the current position
        // in the memory range
        if (good_suffix_skip[j + 1] > bad_char_skip[*(byte*)(startaddress + i + j)]) {
            // If the good suffix skip distance is greater, use it to skip ahead
            // in the search
            i += good_suffix_skip[j + 1];
        }
        else {
            // Otherwise, use the bad character skip distance to skip ahead in the search
            i += bad_char_skip[*(byte*)(startaddress + i + j)];
        }
    }
    // If the search is complete and the signature has not been found, return NULL
    return NULL;
}




DWORD hacks::GetLocalPlayer(std::vector<int> playerSig) {
	DWORD SigStart = GetAddressFromSignature(playerSig, 0x10000000, 0x70000000);

	if (SigStart != NULL) {
		DWORD eax = *(DWORD*)(*(DWORD*)(SigStart + 0x01));
		DWORD edx = *(DWORD*)(*(DWORD*)(SigStart + 0x07));
		return *(DWORD*)(eax + edx * 4 + 0x08);
	}
	else {
		std::cout << "Could not find local player!" << std::endl;
		return NULL;
	}

}

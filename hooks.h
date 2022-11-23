#pragma once
#include "gui.h"

namespace hooks
{
	void Setup();
	void Destroy() noexcept;


	constexpr void* VirtualFunction(void* thisptr, size_t index) noexcept
	{
		return (*static_cast<void***>(thisptr))[index];
	}

	using EndSceneFn = long(__thiscall*)(void*, IDirect3DDevice9*) noexcept;
	inline EndSceneFn EndSceneOriginal = nullptr;
	long __stdcall EndScene(IDirect3DDevice9* device) noexcept;

	using ResetFn = HRESULT(__thiscall*)(void*, IDirect3DDevice9*, D3DPRESENT_PARAMETERS*) noexcept;
	inline ResetFn ResetOriginal = nullptr;
	HRESULT __stdcall Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept;

	
	

	using NewNPCfn = int(__fastcall*)(DWORD, int, int, float, float, float, float, int, int, int);
	inline NewNPCfn NewNPCOriginal = nullptr;
	int __fastcall NewNPC(DWORD source, int X, int Target, float ai3, float ai2, float ai1, float ai0, int Start, int Type, int Y) noexcept;

	
	
}
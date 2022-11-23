#include "hooks.h"
#include <iostream>
#include <stdexcept>
#include <intrin.h>

#include "../ext/minhook/minhook.h"

#include "../ext/imgui/imgui.h"
#include "../ext/imgui/imgui_impl_win32.h"
#include "../ext/imgui/imgui_impl_dx9.h"
#include "hacks.h"

void hooks::Setup()
{
	// Grab Functions to Hook thru sig scan
	hacks::TerrariaSignatures sigs;

	if (MH_Initialize())
		throw std::runtime_error("Unable to initialize minhook");


	if (MH_CreateHook(
		VirtualFunction(gui::device, 42),
		&EndScene,
		reinterpret_cast<void**>(&EndSceneOriginal)
	)) throw std::runtime_error("Unable to hook EndScene()");

	if (MH_CreateHook(
		VirtualFunction(gui::device, 16),
		&Reset,
		reinterpret_cast<void**>(&ResetOriginal)
	)) throw std::runtime_error("Unable to hook Reset()");


	DWORD fNewNPCAddr = hacks::GetAddressFromSignature(sigs.fNewNPC);
	// function is 40 bytes behind the signature
	fNewNPCAddr -= 0x40;
	if (MH_CreateHook(
		reinterpret_cast<void*>(fNewNPCAddr),
		&NewNPC,
		reinterpret_cast<void**>(&NewNPCOriginal)
	)) throw std::runtime_error("Unable to hook fNewNPC()");


	if (MH_EnableHook(MH_ALL_HOOKS))
		throw std::runtime_error("Unable to enable hooks");

	gui::DestroyDirectX;
}

void hooks::Destroy() noexcept
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}



long __stdcall hooks::EndScene(IDirect3DDevice9* device) noexcept
{
	static const auto returnAddress = _ReturnAddress();

	const auto result = EndSceneOriginal(device, device);

	// stop endscene getting called twice

	if (_ReturnAddress() == returnAddress) {
		return result;
	}

	if (!gui::setup)
		gui::SetupMenu(device);

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	gui::RenderESP();

	if (gui::open)
		gui::Render();


	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	return result;
}


HRESULT __stdcall hooks::Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	const auto result = ResetOriginal(device, device, params);
	ImGui_ImplDX9_CreateDeviceObjects();
	return result;
}

// NewNPCfn
int __fastcall hooks::NewNPC(DWORD source, int X, int Target, float ai3, float ai2, float ai1, float ai0, int Start, int Type, int Y) noexcept
{	
	bool verbose = false;
	
	if (verbose) {
		std::cout << "--------------------------" << std::endl;
		std::cout << "NewNPC called" << std::endl;
		std::cout << "Source Address: " << source << std::endl;
		std::cout << "X: " << X << " Y: " << Y << " Type: " << Type << " Start:" << Start << std::endl;
		std::cout << "ai0: " << ai0 << " ai1: " << ai1 << " ai2: " << ai2 << " ai3: " << ai3 << std::endl;
		std::cout << "Target: " << Target << std::endl;
		std::cout << "--------------------------" << std::endl;
	}
	

	// Call original function with random NPC!
	return NewNPCOriginal(source, X, Target, ai3, ai2, ai1, ai0, Start, Type + 5, Y);
}
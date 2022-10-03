#include "gui.h"
#include "../ext/imgui/imgui.h"
#include "../ext/imgui/imgui_impl_win32.h"
#include "../ext/imgui/imgui_impl_dx9.h"
#include "hacks.h"
#include <stdexcept>
#include <string>
#include <iostream>
#include <iostream>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wideParam,
	LPARAM longParam
);

// window process
LRESULT CALLBACK WindowProcess(
	HWND window,
	UINT message,
	WPARAM wideParam,
	LPARAM longParam
);

bool gui::SetupWindowClass(const char* windowClassName) noexcept
{
	//Populate Window Class
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = DefWindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = windowClassName;
	windowClass.hIconSm = NULL;

	// register
	if (!RegisterClassEx(&windowClass))
	{
		return false;
	}

	return true;
}
void gui::DestroyWindowClass() noexcept
{
	UnregisterClass(
		windowClass.lpszClassName,
		windowClass.hInstance
	);
}

bool gui::SetupWindow(const char* windowName) noexcept
{
	// Setup Temp Window
	window = CreateWindow(
		windowClass.lpszClassName,
		windowName,
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		100,
		100,
		0,
		0,
		windowClass.hInstance,
		0
	);
	if (!window) {
		return false;
	}
	return true;
}
void gui::DestroyWindow() noexcept
{
	if (window) {
		DestroyWindow(window);
	}
}

bool gui::SetupDirectX() noexcept
{
	const auto handle = GetModuleHandle("d3d9.dll");

	if (!handle)
	{
		return false;
	}

	using CreateFn = LPDIRECT3D9(__stdcall*)(UINT);

	const auto create = reinterpret_cast<CreateFn>(GetProcAddress(handle, "Direct3DCreate9"));

	if (!create) {
		return false;
	}

	d3d9 = create(D3D_SDK_VERSION);

	if (!d3d9)
	{
		return false;
	}

	D3DPRESENT_PARAMETERS params = { };
	params.BackBufferWidth = 0;
	params.BackBufferHeight = 0;
	params.BackBufferFormat = D3DFMT_UNKNOWN;
	params.BackBufferCount = 0;
	params.MultiSampleType = D3DMULTISAMPLE_NONE;
	params.MultiSampleQuality = NULL;
	params.hDeviceWindow = window;
	params.Windowed = 1;
	params.EnableAutoDepthStencil = 0;
	params.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
	params.Flags = NULL;
	params.FullScreen_RefreshRateInHz = 0;
	params.PresentationInterval = 0;

	if (d3d9->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_NULLREF,
		window,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT,
		&params,
		&device) < 0) return false;

	return true;

}
void gui::DestroyDirectX() noexcept
{
	if (device)
	{
		device->Release();
		device = NULL;
	}
	if (d3d9)
	{
		d3d9->Release();
		d3d9 = NULL;
	}
}


// Setup Device
void gui::Setup()
{
	if (!SetupWindowClass("hackClass001"))
		throw std::runtime_error("Failed to create window class.");
	if (!SetupWindow("Hack Window"))
		throw std::runtime_error("Failed to create window.");

	if (!SetupDirectX())
		throw std::runtime_error("Failed to setup DirectX.");

	DestroyWindow();
	DestroyWindowClass();
}


void gui::SetupMenu(LPDIRECT3DDEVICE9 device) noexcept
{
	auto params = D3DDEVICE_CREATION_PARAMETERS{};
	device->GetCreationParameters(&params);

	window = params.hFocusWindow;

	originalWindowProcess = reinterpret_cast<WNDPROC>(SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProcess)));

	//Imgui
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);

	setup = true;

}
void gui::Destroy() noexcept
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// restore wnd proc
	SetWindowLongPtr(
		window,
		GWLP_WNDPROC,
		reinterpret_cast<LONG_PTR>(originalWindowProcess)
	);

	DestroyDirectX();
}

// Hack Stuff
DWORD Entry = 0;
DWORD LocalPlayerAddr = 0;
DWORD ScreenWidthAddr = 0;
DWORD ScreenHeightAddr = 0;
int ScreenWidth = 0;
int ScreenHeight = 0;
hacks::TerrariaSignatures Sigs;
hacks::PlayerOffsets Offsets;
hacks::InventoryOffsets InvOffsets;
hacks::ItemOffsets ItemOff;
hacks::NpcOffsets NpcOff;
//HackStuff (shouldnt be in gui but)
// Scanning
bool signaturesScanned = false;
bool scanning = false;

bool frozenTime = false;
bool fastForwardTime = false;

bool godMode = false;

float savedX = NULL;
float savedY = NULL;
bool savedPos = false;
bool DrawESP = false;

std::vector<const char*> Logs;
DWORD EntityList = 0;

void DrawEntity(int Index, float PlayerPosX, float PlayerPosY, ImDrawList* pDrawList ) {
	DWORD EntityOne = *(DWORD*)(EntityList + ((Index * 0x4) + 0x8));

	int AmIActive = *(int*)(EntityOne + NpcOff.active);
	if (AmIActive == 0) {
		return;
	}
	//std::cout << "EntityOne Addr = 0x" << std::hex << EntityOne << std::endl;
	//std::cout << "Finding EntityOne Position..." << std::endl;
	float EntityOneX = *(float*)(EntityOne + NpcOff.PosX);
	float EntityOneY = *(float*)(EntityOne + NpcOff.PosY);
	//std::cout << "EntityOneX: " << EntityOneX << std::endl;
	//std::cout << "EntityOneY: " << EntityOneY << std::endl;

	//std::cout << "Finding EntityOne Dimentions..." << std::endl;
	int EntityOneWidth = *(int*)(EntityOne + NpcOff.width);
	int EntityOneHeight = *(int*)(EntityOne + NpcOff.height);
	//std::cout << "EntityOneWidth: " << EntityOneWidth << std::endl;
	//std::cout << "EntityOneHeight: " << EntityOneHeight << std::endl;


	float relativeX = EntityOneX - PlayerPosX;
	float relativeY = EntityOneY - PlayerPosY;

	// We need to make sure the NPC is on screen (within bounds of width/2 height/2)

	if (abs(relativeX) > (ScreenWidth / 2) || (abs(relativeY) > (ScreenHeight / 2)))
	{
		//std::cout << "Can't Draw Offscreen NPC" << std::endl;
	}
	else
	{
		// float topLeft, bottomRight
		//std::cout << "Trying to draw NPC" << std::endl;
		pDrawList->AddRect(
			ImVec2((float)(ScreenWidth / 2) + relativeX - EntityOneWidth, (float)ScreenHeight / 2 + relativeY + (EntityOneHeight / 2)),
			ImVec2((float)(ScreenWidth / 2) + relativeX + (EntityOneWidth / 2), (float)ScreenHeight / 2 + relativeY - (EntityOneHeight / 2)),
			ImColor(255, 192, 180), 0.0f, 0, 1.0f);

	}
}



void gui::RenderESP() noexcept
{
	hacks::NpcOffsets npcOffsets;
	if (signaturesScanned && DrawESP) {
		if (ScreenHeightAddr != 0) {
			//std::cout << "Reading ScreenDimentions" << std::endl;
			ScreenWidth = *(int*)ScreenWidthAddr;
			ScreenHeight = *(int*)ScreenHeightAddr;
			//std::cout << "Screen Width = " << std::dec << ScreenWidth << std::endl;
			//std::cout << "Screen Height = " << std::dec << ScreenHeight << std::endl;
		}

		//std::cout << "Finding PlayerDimentions..." << std::endl;
		int PlayerWidth = *(int*)(LocalPlayerAddr + Offsets.Width);
		int PlayerHeight = *(int*)(LocalPlayerAddr + Offsets.Height);
		//std::cout << std::dec << "Player Width: " << PlayerWidth << std::endl;
		//std::cout << std::dec << "Player Height: " << PlayerHeight << std::endl;

		//std::cout << "Finding PlayerPosition..." << std::endl;
		float PlayerPosX = *(float*)(LocalPlayerAddr + Offsets.PositionX);
		float PlayerPosY = *(float*)(LocalPlayerAddr + Offsets.PositionY);
		//std::cout << "PlayerPosX: " << PlayerPosX << std::endl;
		//std::cout << "PlayerPosY: " << PlayerPosY << std::endl;

		
		//std::cout << "Creating Overlay..." << std::endl;
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(ScreenWidth, ScreenHeight));
		ImGui::Begin("Overlay", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);

		//std::cout << "Creating DrawList..." << std::endl;
		auto pDrawList = ImGui::GetWindowDrawList();


		// Draw Entities

		int NumEntities = *(int*)(EntityList + 0x4);

		for (int i = 0; i < NumEntities; i++) {
			DrawEntity(i, PlayerPosX, PlayerPosY, pDrawList);
		}

		ImGui::End();
	}
}

void gui::Render() noexcept
{

	//Imgui Menu stuff! ~~~~
	ImGui::Begin("1504681 Menu", &open);
	// Show Hacks only if we have signatures scanned
	if (!signaturesScanned) {
		if (ImGui::Button("Start Scan"))
		{
			scanning = true;
		}
		if (scanning)
		{
			ImGui::TextColored(ImVec4(0.0f, 255.0f, 0.0f, 1.00f), "Scanning...");
		}
	}
	else {
		// Once we have all values.

		if (Entry != 0 && LocalPlayerAddr != 0) {
			ImGui::TextColored(ImVec4(0.0f, 244.0f, 0.0f, 100.0f), "Loaded.");
			ImGui::Text("WindowSize: %d x %d", ScreenHeight, ScreenWidth);
		}
		ImGui::BeginGroup();
		ImGui::Text("Game Mods");
		ImGui::Checkbox("Draw ESP", &DrawESP);
		if (ImGui::Checkbox("Freeze Time", &frozenTime))
		{
			if (Entry != NULL) {
				BYTE* freezeTime = (BYTE*)(Entry + 0x02);
				BYTE* fastForwardTime = (BYTE*)(Entry + 0x02);

				if (!frozenTime) {
					memset((void*)freezeTime, 0x74, 1);
				}
				else {
					memset((void*)freezeTime, 0x75, 1);
				}

			}
		}
		if (ImGui::Checkbox("Fastforward Time", &frozenTime))
		{
			if (Entry != NULL) {
				BYTE* freezeTime = (BYTE*)(Entry + 0x02);
				BYTE* fastForwardTime = (BYTE*)(Entry + 0x02);

				if (!frozenTime) {
					memset((void*)freezeTime, 0x74, 1);
				}
				else {
					memset((void*)freezeTime, 0x75, 1);
				}

			}
		}
		ImGui::EndGroup();
		// Begin Player Group
		ImGui::BeginGroup();
		ImGui::Text("Player Mods");
		ImGui::Checkbox("Godmode", &godMode);
		ImGui::SliderInt("Health", (int*)(LocalPlayerAddr + Offsets.Health), 0, *(int*)(LocalPlayerAddr + Offsets.MaxHealth));
		ImGui::SliderFloat("X Position", (float*)(LocalPlayerAddr + Offsets.PositionX), 0, 60000);
		ImGui::SliderFloat("Y Position", (float*)(LocalPlayerAddr + Offsets.PositionY), 600, 25000);
		ImGui::EndGroup();

		// Begin Teleport Group
		ImGui::BeginGroup();
		ImGui::Text("Teleports");
		if (ImGui::Button("Save Co-ords"))
		{
			savedPos = true;
			savedX = *(float*)(LocalPlayerAddr + Offsets.PositionX);
			savedY = *(float*)(LocalPlayerAddr + Offsets.PositionY);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load Co-ords")) {
			*(float*)(LocalPlayerAddr + Offsets.PositionX) = savedX;
			*(float*)(LocalPlayerAddr + Offsets.PositionY) = savedY;
		}
		if (savedPos)
		{
			ImGui::Text("X: %f | ", savedX);
			ImGui::SameLine();
			ImGui::Text("Y: %f", savedY);
		}
		ImGui::EndGroup();

		// Begin Item Group
		ImGui::BeginGroup();
		ImGui::Text("Item Mods");
		ImGui::Text("Item (Slot 4)");
		DWORD InventoryAddr = *(DWORD*)(LocalPlayerAddr + Offsets.InventoryPtr);
		DWORD Slot4 = *(DWORD*)(InventoryAddr + InvOffsets.itemSlot4);
		ImGui::SliderInt("NetID", (int*)(Slot4 + ItemOff.netID), 0, 3000);
		ImGui::SliderInt("StringColor", (int*)(Slot4 + ItemOff.stringColor), 0, 100);
		ImGui::SliderInt("Stack", (int*)(Slot4 + ItemOff.stack), 1, *(int*)(Slot4 + ItemOff.maxStack));
		ImGui::SliderInt("Damage", (int*)(Slot4 + ItemOff.damage), 0, 999);
		ImGui::SliderInt("Knockback", (int*)(Slot4 + ItemOff.knockback), 0, 15);
		ImGui::SliderInt("useTime", (int*)(Slot4 + ItemOff.useTime), 1, 100);
		ImGui::SliderInt("useAnimation", (int*)(Slot4 + ItemOff.useAnimation), 1, 100);

		ImGui::Text("Tool Power");
		ImGui::SliderInt("Axe Power", (int*)(Slot4 + ItemOff.axe), 0, 9999);
		ImGui::SliderInt("Pickaxe Power", (int*)(Slot4 + ItemOff.pick), 0, 9999);
		ImGui::SliderInt("Hammer Power", (int*)(Slot4 + ItemOff.hammer), 0, 9999);
		ImGui::EndGroup();
	}
	//ImGui::ShowStyleEditor();
	ImGui::End();


	
	if (scanning) {
		scanning = false;
		hacks::TerrariaSignatures sigs;
		std::cout << "Scanning for data..." << std::endl;
		std::cout << "Scanning for Time Function..." << std::endl;
		Entry = hacks::GetAddressFromSignature(sigs.TimeInstruction);
		std::cout << "Scanning for LocalPlayer..." << std::endl;
		LocalPlayerAddr = hacks::GetLocalPlayer(sigs.LocalPlayer);
		std::cout << "Scanning for ScreenDimentions..." << std::endl;
		
		// ScreenDimentions
		DWORD ScreenInstruction = hacks::GetAddressFromSignature(sigs.ScreenInstruction);
		std::cout << "Screen Instruction at 0x" << std::hex << ScreenInstruction << std::endl;
		std::cout << "Locating Width & Height..." << std::endl;
		ScreenHeightAddr = *(DWORD*)(ScreenInstruction + 0x01);
		ScreenHeight = *(int*)ScreenHeightAddr;
		std::cout << "Screen Width = " << std::dec << ScreenWidth << std::endl;
		ScreenWidthAddr = *(DWORD*)(ScreenInstruction - 0x50);
		ScreenWidth = *(int*)ScreenWidthAddr;
		std::cout << "Screen Height = " << std::dec << ScreenHeight << std::endl;



		if (Entry != 0 && LocalPlayerAddr != 0) {
			signaturesScanned = true;

			//std::cout << std::dec << "ScreenHeight = " << (int)ScreenHeight << std::endl;
			std::cout << std::hex << "Entry = 0x" << Entry << "| LocalPlayer = 0x" << LocalPlayerAddr << std::dec << std::endl;
			//printf("ScreenWidthAddr = %d | ScreenHeightAddr = %d", ScreenWidthAddr, ScreenHeightAddr);
			// EntityArray
			DWORD* EntityArrayPtr = *(DWORD**)(hacks::GetAddressFromSignature(Sigs.NPCArray) - 0xD);
			std::cout << "NPC List Ptr: 0x" << std::hex << EntityArrayPtr << std::dec << std::endl;
			EntityList = *EntityArrayPtr;
			std::cout << "NPC List Address: 0x" << std::hex << EntityList << std::dec << std::endl;
			// Try Drawing an NPC

			
		}

		// Hook NewNPCFn

		
	}
}


LRESULT CALLBACK WindowProcess(
	HWND window,
	UINT message,
	WPARAM wideParam,
	LPARAM longParam
)
{


	// Constant Writes
	if (godMode)
	{
		*(int*)(LocalPlayerAddr + Offsets.Health) = *(int*)(LocalPlayerAddr + Offsets.MaxHealth);
	}

	// toggle menu
	if (GetAsyncKeyState(VK_INSERT) & 1) {
		gui::open = !gui::open;
	}

	// Pass Messages to Imgui
	if (gui::open && ImGui_ImplWin32_WndProcHandler(
		window,
		message,
		wideParam,
		longParam
	)) return 1L;



	return CallWindowProc(
		gui::originalWindowProcess,
		window,
		message,
		wideParam,
		longParam
	);
}

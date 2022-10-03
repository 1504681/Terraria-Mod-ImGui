# Terraria-Mod-ImGui
An interactive menu for terraria. (Hooked DirectX with MinHook, Graphics with ImGui)

## Update: Oct 3rd, 2022

- NPCList Signature w/ Basic ESP on NPC[0]
- Next: Building a more simple function for ESP, and iterating through entire NPCList array.
- Next: Draw Health/MaxHealth as a % as a HealthBar Above ESP Box

Project Goals:
- Learn how to hook DirectX9
- Learn how to use ImGui & MinHook libraries
- Build a clean, functional and useful menu for tinkering in-game.

![image](https://user-images.githubusercontent.com/84855585/188251960-770d7741-71ba-4bb2-b0ca-5abc1cae3123.png)


- On Initial Load, the menu displays a button to start scanning for LocalPlayer Address & Address of the Terraria::Time function
- Searches for LocalPlayer from BYTE signature, "-1" signifying wildcard bytes: [ 0xA1,  -1, -1, -1, -1, 0x8B, 0x15, -1, -1, -1, -1, 0x3B, 0x50, 0x04, 0x73, 0x05, 0x8B, 0x44, 0x90, 0x08, 0xC3, 0xE8 ] 
- Using GetAddressFromSignature function and GetLocalPlayer function (hacks.cpp), finds these addresses


GetLocalPlayer does the same function of the Terraria::GetLocalPlayer function

Ingame Function:

![image](https://user-images.githubusercontent.com/84855585/188252251-2823387e-6f05-43e7-ae32-637a7890090a.png)

Our Function:

![image](https://user-images.githubusercontent.com/84855585/188252194-efbc6c7f-ca62-4474-b794-0a237733a116.png)


Most of the 'Mod' implementations take place in gui.cpp, in the gui::Render() function. (Learning ImGui was the goal of the project)

The Freezing Time mod is patching a jmp instruction with memset:

![image](https://user-images.githubusercontent.com/84855585/188252686-17dc560b-e48b-426a-b394-c2f7d53ed384.png)

Player Mods are interactable sliders to change Health, PositionX and PositionY.

![image](https://user-images.githubusercontent.com/84855585/188252755-a0b9f2c3-300e-4cb6-8745-a50819f2ed42.png)

![image](demos/1loXkd0.gif)

Items are accessed from : PlayerAddr->Inventory->ItemIndex

![image](https://user-images.githubusercontent.com/84855585/188252837-cc54a728-bbb1-489c-82cd-695ed0588536.png)

![image](https://user-images.githubusercontent.com/84855585/188252848-eb083ef7-1c1e-4253-8c72-1fc46706b5aa.png)

ImGui makes tweaking items a breeze:

![image](demos/NlZFqWE.gif)


// Sources:

// ImGui & DirectX Hooking: https://www.youtube.com/watch?v=Nrta_J_c9Cc

// Hacking Terraria with C++: https://www.youtube.com/watch?v=uuMg7CeJF1k

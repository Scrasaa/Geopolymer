#pragma once

#include "../../pch.h"
#include "../../ValveSDK/Interfaces/CHLClient.h"
#include "../../ValveSDK/Interfaces/IClientEntityList.h"
#include "../../ValveSDK/Interfaces/IVModelRender.h"
#include "../../ValveSDK/Interfaces/IVRenderView.h"
#include "../../ValveSDK/Interfaces/IMaterialSystem.h"

namespace Interface
{
	extern IBaseClientDLL* BaseClient;
	extern IClientEntityList* EntityList;
	extern IVModelRender* ModelRender;
	extern IVRenderView* RenderView;
	extern IMaterialSystem* MaterialSystem;
	extern IStudioRender* StudioRender;
	extern IVEngineClient* EngineClient;
}

// Function Pointer Declaration
using pCreateInterface = PVOID (*)(char* szInterfaceName, int* retValue);

class CInterface
{
	template<typename T>
	T GetInterface(pCreateInterface oFunc, char* szInterfaceName);
	pCreateInterface GetModuleFactory(HMODULE hModule);
public:
	void Initialize();
};

extern CInterface* g_Interface;
#include "CInterface.h"
#include "../../Thirdparty/xorstr.h"
#include "../../CustomSDK/Memory/CMemory.h"

// -- Declaration -- // 
CInterface* g_Interface = nullptr;

IBaseClientDLL* Interface::BaseClient = nullptr;
IClientEntityList* Interface::EntityList = nullptr;
IVModelRender* Interface::ModelRender = nullptr;
IVRenderView* Interface::RenderView = nullptr;
IMaterialSystem* Interface::MaterialSystem = nullptr;
IStudioRender* Interface::StudioRender = nullptr;
IVEngineClient* Interface::EngineClient = nullptr;

template<typename T>
inline T CInterface::GetInterface(pCreateInterface oFunc, char* szInterfacename)
{
	return reinterpret_cast<T>(oFunc(szInterfacename, nullptr));
}

pCreateInterface CInterface::GetModuleFactory(HMODULE hModule)
{
	// if we want to use by hash here is the CreateInterface hash 0x62d3845
	auto oInterfaceAddy = reinterpret_cast<FARPROC>(g_Memory->GetFunctionAddress(xorstr("CreateInterface"), hModule));

	if (!oInterfaceAddy)
		return nullptr;

	return reinterpret_cast<pCreateInterface>(oInterfaceAddy);
}

void CInterface::Initialize()
{
	auto clientFactory = GetModuleFactory(g_Memory->ResolveModuleBaseAddressPEB(xorstr("client.dll")));
	auto engineFactory = GetModuleFactory(g_Memory->ResolveModuleBaseAddressPEB(xorstr("engine.dll")));
	auto materialFactory = GetModuleFactory(g_Memory->ResolveModuleBaseAddressPEB(xorstr("MaterialSystem.dll")));
	auto studioFactory = GetModuleFactory(g_Memory->ResolveModuleBaseAddressPEB(xorstr("studiorender.dll")));

	// -- Initializing Interfaces -- // 
	Interface::BaseClient = GetInterface<IBaseClientDLL*>(clientFactory, xorstr("VClient017"));
	Interface::EntityList = GetInterface<IClientEntityList*>(clientFactory, xorstr("VClientEntityList003"));
	Interface::ModelRender = GetInterface<IVModelRender*>(engineFactory, xorstr("VEngineModel016"));
	Interface::RenderView = GetInterface<IVRenderView*>(engineFactory, xorstr("VEngineRenderView014"));
	Interface::MaterialSystem = GetInterface<IMaterialSystem*>(materialFactory, xorstr("VMaterialSystem080"));
	Interface::StudioRender = GetInterface<IStudioRender*>(studioFactory, xorstr("VStudioRender025"));
	Interface::EngineClient = GetInterface<IVEngineClient*>(engineFactory, xorstr("VEngineClient013"));
}
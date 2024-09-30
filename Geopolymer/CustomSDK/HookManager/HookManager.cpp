#include "HookManager.h"
#include "../../Thirdparty/MinHook/MinHook.h"
#include "../../Thirdparty/xorstr.h"

#include "../Memory/CMemory.h"
#include "../InterfaceManager/CInterface.h"
#include "../MyPlayer/CLocalPlayer.h"

#include "../../ValveSDK/Includes/usercmd.h"
#include "../../ValveSDK/Interfaces/IVModelRender.h"
#include "../../ValveSDK/Interfaces/IMaterialSystem.h"
#include "../../ValveSDK/Entities/C_CSPlayer.h"

typedef bool(__thiscall* tCreateMove)(void*, float, CUserCmd*);
tCreateMove oCreateMove = nullptr;

bool __fastcall hkCreateMove(void* thisptr, void* edx, float flInputSampleTime, CUserCmd* pCmd)
{
	if (!pCmd || !pCmd->command_number)
		return oCreateMove(thisptr, flInputSampleTime, pCmd);

	g_LocalPlayer.Get(&g_LocalPlayer);

	return false;
}

typedef void(__thiscall* tDrawModelExecute)(void*, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* pBoneToWorld);
tDrawModelExecute oDrawModelExecute = nullptr;

static float clearColor[3] = { 1.f,1.f,1.f };

void __fastcall hkDrawModelExecute(void* thisptr, void* edx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
{
	g_LocalPlayer.Get(&g_LocalPlayer);

	if (!g_LocalPlayer.pEntity)
		return oDrawModelExecute(thisptr, pState, pInfo, pCustomBoneToWorld);

	auto pEnt = (C_BasePlayer*)Interface::EntityList->GetClientEntity(pInfo.entity_index);

	if (!pEnt)
		return oDrawModelExecute(thisptr, pState, pInfo, pCustomBoneToWorld);

	std::string modelName = pInfo.pModel->name;

	if (modelName.find(xorstr("models/player")) != std::string::npos && pEnt->IsPlayer() && pEnt->IsAlive() && pEnt != g_LocalPlayer.pEntity)
	{
		auto pRenderContext = (IMatRenderContext*)Interface::MaterialSystem->GetRenderContext();
		// ignore z
		if (pRenderContext)
			pRenderContext->DepthRange(0.f, 0.f);

		bool bDrawOriginal = true;

		if (bDrawOriginal)
			oDrawModelExecute(thisptr, pState, pInfo, pCustomBoneToWorld);
		/*
		// material override
		IMaterial* DebugWhite = Interface::MaterialSystem->FindMaterial(xorstr("vgui/white_additive"), xorstr(TEXTURE_GROUP_VGUI));
		DebugWhite->SetMaterialVarFlag(MATERIAL_VAR_FLAT, true);
		DebugWhite->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
		DebugWhite->SetMaterialVarFlag(MATERIAL_VAR_SELFILLUM, true);
		Interface::ModelRender->SuppressEngineLighting(true);
		Interface::ModelRender->SetupLighting(Vector());

		Interface::RenderView->SetColorModulation(0.f, 0.f, 255.f);
		Interface::RenderView->SetBlend(0.4f);
		Interface::ModelRender->ForcedMaterialOverride(DebugWhite);

		oDrawModelExecute(thisptr, pState, pInfo, pCustomBoneToWorld);

		// visible

		DebugWhite->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
		DebugWhite->SetMaterialVarFlag(MATERIAL_VAR_SELFILLUM, false);
		Interface::RenderView->SetBlend(0.7f);
		Interface::RenderView->SetColorModulation(255.f, 0.f, 0.f);
		Interface::ModelRender->ForcedMaterialOverride(DebugWhite);

		oDrawModelExecute(thisptr, pState, pInfo, pCustomBoneToWorld);

		*/

		// Clean Up
		/*
		Interface::ModelRender->SuppressEngineLighting(false);

		Interface::RenderView->SetColorModulation(clearColor);
		Interface::RenderView->SetBlend(1.f);

		Interface::ModelRender->ForcedMaterialOverride(nullptr);
		*/

		if (pRenderContext)
			pRenderContext->DepthRange(0.f, 1.f);

		return;

	} 

/*
	* 
	if (pInfo.pModel)
	{
		std::string modelName = pInfo.pModel->name;

		if (modelName.find("models/player") != std::string::npos)
		{
			
		}
		else if (modelName.find("flash") != std::string::npos)
		{
			IMaterial* FlashWhite = Interface::MaterialSystem->FindMaterial(xorstr("effects\\flashbang_white"), xorstr(TEXTURE_GROUP_CLIENT_EFFECTS));
			FlashWhite->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			Interface::ModelRender->ForcedMaterialOverride(FlashWhite);
		}
		else if (modelName.find("models/weapons/") != std::string::npos)
		{
			IMaterial* test = Interface::MaterialSystem->FindMaterial(xorstr("models/debug/debugwhite"), xorstr(TEXTURE_GROUP_MODEL));
			test->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
			Interface::RenderView->SetBlend(1.0f);
			Interface::RenderView->SetColorModulation(0.f, 255.f, 0.f);
			Interface::ModelRender->ForcedMaterialOverride(test);
		}
		oDrawModelExecute(thisptr, pState, pInfo, pCustomBoneToWorld);
	}

	Interface::RenderView->SetColorModulation(1.f,1.f,1.f);
	Interface::ModelRender->ForcedMaterialOverride(nullptr);
*/
	oDrawModelExecute(thisptr, pState, pInfo, pCustomBoneToWorld);
}

bool HookManager::Initialize()
{
	if (MH_Initialize() != MH_OK)
		return false;

	if (MH_CreateHook((LPVOID)g_PatternScan->InPatternScan(xorstr("55 8B EC E8 ? ? ? ? 8B C8 85 C9 75 06"), xorstr("client.dll")), &hkCreateMove, (LPVOID*)&oCreateMove) != MH_OK)
		return false;

	if (MH_CreateHook((LPVOID)g_Memory->GetVirtual((void***)Interface::ModelRender, 19), &hkDrawModelExecute, (LPVOID*)&oDrawModelExecute) != MH_OK)
		return false;

	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
		return false;

	return true;
}


bool HookManager::Destroy()
{
	if (MH_DisableHook(MH_ALL_HOOKS) != MH_OK)
		return false;

	if (MH_RemoveHook(MH_ALL_HOOKS) != MH_OK)
		return false;

	if (MH_Uninitialize() != MH_OK)
		return false;

	return true;
}
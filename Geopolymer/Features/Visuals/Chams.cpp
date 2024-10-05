#include "Chams.h"
#include "../../pch.h"
#include "../../CustomSDK/InterfaceManager/CInterface.h"
#include "../../ValveSDK/Interfaces/IMaterialSystem.h"
#include "../../ValveSDK/Includes/KeyValues.h"

static float clearColor[4] = { 1.f,1.f,1.f, 1.f };

IMaterial* Chams::CreateMaterial()
{
    IMaterial* pMat = nullptr;

    auto* kv = new KeyValues("VertexLitGeneric");
    kv->SetString("$dummy", "dummy");
    kv->SetString("$basetexture", "vgui/white_additive");
    kv->SetString("$bumpmap", "vgui/white_additive");
    kv->SetString("$selfillum", "1");
    kv->SetString("$selfillumFresnel", "1");
    kv->SetString("$selfillumFresnelMinMaxExp", "[0.4999 0.5 1]");
    kv->SetString("$cloakPassEnabled", "1");
    kv->SetString("$nodecal", "1");
    kv->SetString("$model", "1");

    pMat = Interface::MaterialSystem->CreateMaterial("material_flats", kv);

    pMat->IncrementReferenceCount();

	return pMat;
}

void Chams::PushIgnoreZ()
{
    auto pRenderCtx = (IMatRenderContext*)Interface::MaterialSystem->GetRenderContext();

    if (!pRenderCtx)
        return;

    pRenderCtx->DepthRange(0.f, 0.f);
}

void Chams::PopIgnoreZ()
{
    auto pRenderCtx = (IMatRenderContext*)Interface::MaterialSystem->GetRenderContext();

    if (!pRenderCtx)
        return;

    pRenderCtx->DepthRange(0.f, 1.f);
}

void Chams::PushMaterialOverride(float color[4])
{
    IMaterial* pMat = CreateMaterial();

    if (!pMat)
        return;

    Interface::ModelRender->SuppressEngineLighting(true);
    Interface::ModelRender->SetupLighting(Vector());

    Interface::RenderView->SetColorModulation(color);
    Interface::RenderView->SetBlend(color[3]);

    Interface::ModelRender->ForcedMaterialOverride(pMat);
}

void Chams::PopMaterialOverride()
{
    Interface::ModelRender->SuppressEngineLighting(false);
    
    Interface::RenderView->SetColorModulation(clearColor);
    Interface::RenderView->SetBlend(1.f);
    
    Interface::ModelRender->ForcedMaterialOverride(nullptr);
}


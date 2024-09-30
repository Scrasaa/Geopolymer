#include "Chams.h"
#include "../../pch.h"
#include "../../CustomSDK/InterfaceManager/CInterface.h"
#include "../../ValveSDK/Interfaces/IMaterialSystem.h"

IMaterial* Chams::CreateMaterial(const char* szName, bool bFlat, bool bWireFrame, bool bAddShine)
{
    std::string material_data = std::format(R"("{}"
    {{
        "$basetexture" "vgui/white_additive"
        "$envmap" "{}"
        "$normalmapalphaenvmapmask" "{}"
        "$envmapcontrast" "{}"
        "$model" "1"
        "$flat" "1"
        "$nocull" "0"
        "$selfillum" "1"
        "$halflambert" "1"
        "$nofog" "0"
        "$ignorez" "0"
        "$znearer" "0"
        "$wireframe" "{}"
    }}
    )", bFlat || bWireFrame ? "UnlitGeneric" : "VertexLitGeneric", bAddShine ? "env_cubemap" : "", bAddShine ? 1 : 0, bAddShine ? 1 : 0, bWireFrame ? 1 : 0);

    void* pKeyValues = nullptr;

	return nullptr;
}


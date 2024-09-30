#pragma once

class IMaterial;

namespace Chams
{
	IMaterial* CreateMaterial(const char* szName, bool bFlat, bool bWireFrame, bool bAddShine);

	void PushIgnoreZ(bool bIgnoreZ);
	void PopIgnoreZ();

	void PushMaterialOverride(float color[4], int iMaterialType);
	void PopMaterialOverride();
}
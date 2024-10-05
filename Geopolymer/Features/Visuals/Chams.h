#pragma once

class IMaterial;

namespace Chams
{
	IMaterial* CreateMaterial();

	void PushIgnoreZ();
	void PopIgnoreZ();

	void PushMaterialOverride(float color[4]);
	void PopMaterialOverride();
}
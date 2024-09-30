#pragma once
#include "C_BasePlayer.h"
#include "C_WeaponCSBase.h"

class C_WeaponCSBase;

class ICSPlayerAnimStateHelpers
{
public:
	virtual C_WeaponCSBase* CSAnim_GetActiveWeapon() = 0;
	virtual bool CSAnim_CanMove() = 0;
};

class C_CSPlayer : public C_BasePlayer, public ICSPlayerAnimStateHelpers
{
public:
	virtual void destr0012341() = 0;

	//no virtuals?

public:
	inline C_WeaponCSBase* GetActiveCSWeapon()
	{
		C_BaseCombatWeapon* pWeapon = GetActiveWeapon();

		return pWeapon ? pWeapon->As<C_WeaponCSBase*>() : nullptr;
	}
};
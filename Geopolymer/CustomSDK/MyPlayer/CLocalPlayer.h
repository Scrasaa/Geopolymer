#pragma once
#include "../../ValveSDK/Entities/C_BasePlayer.h"

class MyPlayer
{
public:
	C_BasePlayer* pEntity;
private:
	unsigned int* m_pPredictionRandomSeed;
	float m_fOldCurTime;
	float m_fOldFrameTime;
public: // public functions

	MyPlayer();
	C_BasePlayer* Get(MyPlayer* dest);
	C_BasePlayer* GetLocalPlayer();
};

extern MyPlayer g_LocalPlayer;
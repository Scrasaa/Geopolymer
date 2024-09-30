#include "CLocalPlayer.h"
#include "../InterfaceManager/CInterface.h"

MyPlayer g_LocalPlayer;

MyPlayer::MyPlayer()
{
	this->pEntity = nullptr;
	this->m_pPredictionRandomSeed = nullptr;
	this->m_fOldCurTime = 0.0f;
	this->m_fOldFrameTime = 0.0f;
}

C_BasePlayer* MyPlayer::Get(MyPlayer* dest)
{
	auto LP = Interface::EngineClient->GetLocalPlayer();

	C_BasePlayer* pNewPlayer = LP ? reinterpret_cast<C_BasePlayer*>(Interface::EntityList->GetClientEntity(LP)) : nullptr;

	if (pNewPlayer != dest->pEntity)
		dest->pEntity = pNewPlayer;

	return pNewPlayer;
}

inline C_BasePlayer* MyPlayer::GetLocalPlayer()
{
	return reinterpret_cast<C_BasePlayer*>(Interface::EntityList->GetClientEntity(Interface::EngineClient->GetLocalPlayer()));
}
#include "chatskin.h"

#include <game/client/gameclient.h>
#include <engine/storage.h>
#include <game/generated/protocol.h>
#include <engine/console.h>
#include <cstdio>
#include <cstring>

CChatskin::CChatskin()
{
	m_aTriggerMessage[0] = '\0';
}

void CChatskin::OnConsoleInit()
{
	Console()->Register("chatskin", "?r[message]", CFGFLAG_CLIENT,
		ConMessage, this,
		"사용자가 설정한 메세지를 다른 사람이 보내면 그 메세지를 보낸 사람의 스킨으로 변경됩니다.");
}

void CChatskin::ConMessage(IConsole::IResult *pResult, void *pUserData)
{
	CChatskin *pSelf = static_cast<CChatskin *>(pUserData);

	const char *pInput = pResult->GetString(0);

	std::strncpy(pSelf->m_aTriggerMessage, pInput, sizeof(pSelf->m_aTriggerMessage) - 1);
  pSelf->m_aTriggerMessage[sizeof(pSelf->m_aTriggerMessage) - 1] = '\0';
}

void CChatskin::OnMessage(int Msg, void *pRawMsg)
{
	if (Msg != NETMSGTYPE_SV_CHAT)
		return;

	CNetMsg_Sv_Chat *pMsg = static_cast<CNetMsg_Sv_Chat *>(pRawMsg);

  if (pMsg->m_ClientId < 0)
      return;

	const char *pText = pMsg->m_pMessage;
	int pClientID = pMsg->m_ClientId;

	const char *pName = m_pClient->m_aClients[pClientID].m_aName;
	const char *pSkinName = m_pClient->m_aClients[pClientID].m_aSkinName;
	int pSkinUseCustomColors = m_pClient->m_aClients[pClientID].m_UseCustomColor;
	int pSkinBody = m_pClient->m_aClients[pClientID].m_ColorBody;
	int pSkinFeet = m_pClient->m_aClients[pClientID].m_ColorFeet;

	if (pMsg->m_Team == 0 || pMsg->m_Team == 1)
	{
		if (str_find_nocase(pText, m_aTriggerMessage))
		{
			str_copy(Config()->m_ClPlayerSkin, pSkinName, sizeof(Config()->m_ClPlayerSkin));
			Config()->m_ClPlayerUseCustomColor = pSkinUseCustomColors;
			Config()->m_ClPlayerColorBody = pSkinBody;
			Config()->m_ClPlayerColorFeet = pSkinFeet;

			m_pClient->SendInfo(false);
		}
	}
}
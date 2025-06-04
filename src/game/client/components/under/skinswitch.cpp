#include "skinswitch.h"

#include <game/client/gameclient.h>
#include <engine/console.h>
#include <game/generated/protocol.h>
#include <base/system.h>

void CSkinswitch::OnConsoleInit()
{
    Console()->Register(
        "skin", "r[playername]", CFGFLAG_CLIENT,
        ConSkinChange, this,
        "해당 플레이어와 같은 스킨으로 변경합니다");
}

void CSkinswitch::ConSkinChange(IConsole::IResult *pResult, void *pUserData)
{
    CSkinswitch *pSelf = static_cast<CSkinswitch *>(pUserData);
    const char *pTargetName = pResult->GetString(0);
    if (!pTargetName[0])
    {
        return;
    }

    CGameClient *pClient = pSelf->m_pClient;

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
				const char *pName = pClient->m_aClients[i].m_aName;
        const char *pSkinName = pClient->m_aClients[i].m_aSkinName;
				const int pSkinUseCustomColor = pClient->m_aClients[i].m_UseCustomColor;
				const int pSkinBody = pClient->m_aClients[i].m_ColorBody;
				const int pSkinFeet = pClient->m_aClients[i].m_ColorFeet;
				
        if (str_find_nocase(pName, pTargetName))
        {
          str_copy(pSelf->Config()->m_ClPlayerSkin, pSkinName, sizeof(pSelf->Config()->m_ClPlayerSkin));
					if (pSkinUseCustomColor == 0)
					{
            pSelf->Config()->m_ClPlayerUseCustomColor = pSkinUseCustomColor;
            pSelf->GameClient()->SendInfo(false);

            char aBuf[128];
            std::snprintf(aBuf, sizeof(aBuf), "newSkin='%s' useColor=%d", 
              pSelf->Config()->m_ClPlayerSkin,
              pSelf->Config()->m_ClPlayerUseCustomColor);
            pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "skinswitch", aBuf); 

            return;
					}
					else if (pSkinUseCustomColor == 1)
					{
						pSelf->Config()->m_ClPlayerUseCustomColor = pSkinUseCustomColor;
            pSelf->Config()->m_ClPlayerColorBody      = pSkinBody;
            pSelf->Config()->m_ClPlayerColorFeet      = pSkinFeet;
            pSelf->GameClient()->SendInfo(false);

            char aBuf[128];
            std::snprintf(aBuf, sizeof(aBuf), "newSkin='%s' useColor=%d body=%d feet=%d", 
              pSelf->Config()->m_ClPlayerSkin,
              pSelf->Config()->m_ClPlayerUseCustomColor,
              pSelf->Config()->m_ClPlayerColorBody,
              pSelf->Config()->m_ClPlayerColorFeet);
              
            pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "skinswitch", aBuf);

            return;
					}
        }
    }

    pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "skin",
        "플레이어를 찾을 수 없습니다.");
}

void CSkinswitch::ChangeSkinByName(const char *pTargetName)
{
  CGameClient *pGameClient = GameClient();

  for (int i = 0; i < MAX_CLIENTS; i++)
  {
    const CNetObj_PlayerInfo *pInfo = pGameClient->m_Snap.m_apPlayerInfos[i];
    if (!pInfo)
      continue;

    int clientId = pInfo->m_ClientId;

    const char *pName = pGameClient->m_aClients[clientId].m_aName;
    const char *pSkinName = pGameClient->m_aClients[clientId].m_aSkinName;
		const int pSkinUseCustomColor = pGameClient->m_aClients[clientId].m_UseCustomColor;
		const int pSkinBody = pGameClient->m_aClients[clientId].m_ColorBody;
		const int pSkinFeet = pGameClient->m_aClients[clientId].m_ColorFeet;

    if (str_comp_nocase(pTargetName, "언더") == 0)
    {
      str_copy(pGameClient->Config()->m_ClPlayerSkin, "default_flower", sizeof(pGameClient->Config()->m_ClPlayerSkin));

      Config()->m_ClPlayerUseCustomColor = 1;
      Config()->m_ClPlayerColorBody = 13827960;
      Config()->m_ClPlayerColorFeet = 10748159;
      pGameClient->SendInfo(false);

      return;
    }

    if (str_comp_nocase(pName, pTargetName) == 0)
    {
      str_copy(pGameClient->Config()->m_ClPlayerSkin, pSkinName, sizeof(pGameClient->Config()->m_ClPlayerSkin));

      if (pSkinUseCustomColor == 0)
      {
        Config()->m_ClPlayerUseCustomColor = pSkinUseCustomColor;
        pGameClient->SendInfo(false);

        return;
      }
      else if (pSkinUseCustomColor == 1)
      {
        Config()->m_ClPlayerUseCustomColor = pSkinUseCustomColor;
        Config()->m_ClPlayerColorBody = pSkinBody;
        Config()->m_ClPlayerColorFeet = pSkinFeet;
        pGameClient->SendInfo(false);

        return;
      }
    }
  }
}
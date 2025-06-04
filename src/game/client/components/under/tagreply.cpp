#include "tagreply.h"

#include <game/client/gameclient.h>
#include <engine/console.h>
#include <engine/storage.h>
#include <game/generated/protocol.h>
#include <cstdio>
#include <cstring>

static bool LineShouldHighlight(const char *pLine, const char *pName)
{
    const char *pHit = str_find_nocase(pLine, pName);
    while(pHit)
    {
        int Length = str_length(pName);
        if(Length > 0 && (pLine == pHit || pHit[-1] == ' ') && (pHit[Length] == 0 || pHit[Length] == ' ' || pHit[Length] == '.' || pHit[Length] == '!' || pHit[Length] == ',' || pHit[Length] == '?' || pHit[Length] == ':'))
					return true;

        pHit = str_find_nocase(pHit + 1, pName);
    }
    return false;
}

CTagreply::CTagreply()
{
    m_aTagMsg[0] = '\0';
}

void CTagreply::OnInit()
{
    LoadConfig();
}

void CTagreply::OnConsoleInit()
{
    Console()->Register(
        "tagreply", "?r[message]", CFGFLAG_CLIENT | CFGFLAG_SAVE,
        ConTagreply, this,
        "사용자가 본인을 태그했을 때 자동으로 응답합니다");
}

void CTagreply::ConTagreply(IConsole::IResult *pResult, void *pUserData)
{
    CTagreply *pSelf = static_cast<CTagreply *>(pUserData);

    if (pResult->NumArguments() == 0)
    {
        if (pSelf->m_aTagMsg[0] != '\0')
        {
            char aBuf[512];
            std::snprintf(aBuf, sizeof(aBuf), "\"%s\"", pSelf->m_aTagMsg);
            pSelf->Console()->Print(
                IConsole::OUTPUT_LEVEL_STANDARD, "tagreply", aBuf);
        }
        return;
    }

    const char *pMsgText = pResult->GetString(0);
    if (pMsgText[0] == '\0')
    {
        pSelf->m_aTagMsg[0] = '\0';
    }
    else
    {
        std::strncpy(pSelf->m_aTagMsg, pMsgText, sizeof(pSelf->m_aTagMsg) - 1);
        pSelf->m_aTagMsg[sizeof(pSelf->m_aTagMsg) - 1] = '\0';
    }
    pSelf->SaveConfig();
}

void CTagreply::OnMessage(int Msg, void *pRawMsg)
{
    if (Msg != NETMSGTYPE_SV_CHAT)
        return;

    CNetMsg_Sv_Chat *pMsg = static_cast<CNetMsg_Sv_Chat *>(pRawMsg);

    if (pMsg->m_ClientId < 0)
        return;

    if (m_aTagMsg[0] == '\0')
        return;

    const char *pText = pMsg->m_pMessage;
    const char *pMyName = Client()->PlayerName();
    if (!pMyName || pMyName[0] == '\0')
        return;

		int SenderId = pMsg->m_ClientId;
		const char *pSenderName = m_pClient->m_aClients[SenderId].m_aName;
		if(!pSenderName || pSenderName[0] == '\0')
			return;

        char aBuf[256];
		if (pMsg->m_Team == TEAM_WHISPER_RECV)
		{
            str_format(aBuf, sizeof(aBuf), "/w %s %s", pSenderName, m_aTagMsg);
		    m_pClient->m_Chat.SendChat(0, aBuf);
		}
        else
        {
            str_format(aBuf, sizeof(aBuf), "%s: %s", pSenderName, m_aTagMsg);
            m_pClient->m_Chat.SendChat(0, aBuf);
        }

    
}

void CTagreply::LoadConfig()
{
    IOHANDLE io = Storage()->OpenFile(CfgFilename(), IOFLAG_READ, IStorage::TYPE_SAVE);
    if (!io)
        return;

    char buffer[sizeof(m_aTagMsg)] = {0};
    int read = io_read(io, buffer, sizeof(buffer) - 1);
    io_close(io);
    if (read > 0)
    {
        int len = read;
        if (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')
            buffer[--len] = '\0';
        str_copy(m_aTagMsg, buffer, sizeof(m_aTagMsg));
    }
}

void CTagreply::SaveConfig() const
{
    IOHANDLE io = Storage()->OpenFile(
        CfgFilename(),
        IOFLAG_WRITE,
        IStorage::TYPE_SAVE
    );
    if (!io)
    {
        Console()->Print(
            IConsole::OUTPUT_LEVEL_STANDARD, "tagreply",
            "under_tagreply.cfg 파일을 열 수 없습니다.");
        return;
    }
    io_write(io, m_aTagMsg, std::strlen(m_aTagMsg));
    io_close(io);
}

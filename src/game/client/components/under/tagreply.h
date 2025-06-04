#ifndef GAME_CLIENT_COMPONENTS_UNDER_TAGREPLY_H
#define GAME_CLIENT_COMPONENTS_UNDER_TAGREPLY_H

#include <game/client/component.h>
#include <engine/console.h>

class CTagreply : public CComponent
{
	public:
		CTagreply();

		virtual void OnInit() override;
		virtual void OnConsoleInit() override;
		virtual void OnMessage(int Msg, void *pRawMsg) override;

		static void ConTagreply(IConsole::IResult *pResult, void *pUserData);

		virtual int Sizeof() const override { return sizeof(*this); }

	private:
		char m_aTagMsg[256];

		const char *CfgFilename() const { return "under_tagreply.cfg"; }

		void LoadConfig();
		void SaveConfig() const;
};

#endif
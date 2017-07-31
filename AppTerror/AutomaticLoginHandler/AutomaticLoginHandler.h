#ifndef TERROR_AUTOMATIC_LOGIN_HANDLER_H
#define TERROR_AUTOMATIC_LOGIN_HANDLER_H

#include "LoginSaver/LoginSaver.h"
#include "PlayerProfile/PlayerProfile.h"

namespace terror {
	class AutomaticLoginHandler : public NetworkObserver {
	public:
		AutomaticLoginHandler(GuiLayoutPtr loginLayout, GuiLayoutPtr mainLayout, GuiLayoutPtr activationLayout, LoginSaverPtr loginSaver, PlayerProfilePtr playerProfile, TerrorClientPtr terrorClient)
			: NetworkObserver(false), loginLayout(loginLayout), mainLayout(mainLayout), activationLayout(activationLayout), loginSaver(loginSaver), playerProfile(playerProfile), terrorClient(terrorClient)
		{}

		void tryLogin() {
			if (!this->loginSaver->tryLoad()) {
				return;
			}
			string usernameOrEmail = this->loginSaver->login;
			string password = this->loginSaver->password;
			if (this->validateLogin(usernameOrEmail, password)) {
				SteamUdpAddressPtr steamUdpAddress(new SteamUdpAddress(CSteamID()));
				if (SteamUser()) {
					steamUdpAddress = SteamUdpAddressPtr(new SteamUdpAddress(SteamUser()->GetSteamID()));
				}
				LoginRequestPtr loginRequest(new LoginRequest(usernameOrEmail, password));
				this->terrorClient->sendObject(loginRequest);
				this->loginLayout->disable();
			}
		}

		virtual ~AutomaticLoginHandler(){}

	protected:
		GuiLayoutPtr loginLayout;
		LoginSaverPtr loginSaver;
		GuiLayoutPtr mainLayout;
		GuiLayoutPtr activationLayout;
		PlayerProfilePtr playerProfile;
		TerrorClientPtr terrorClient;

	private:
		virtual void handleTcpPacket(TcpPacketPtr tcpPacket) {
			if (tcpPacket->getLabel() == LOGIN_RESPONSE) {
				LoginResponsePtr loginResponse(new LoginResponse);
				tcpPacket->buildObject(loginResponse);

				this->loginLayout->enable();
				if (loginResponse->getLoginError() == NO_LOGIN_ERROR) {
					this->playerProfile->setUsername(loginResponse->getUsername());
					this->playerProfile->setEmail(loginResponse->getEmail());
					this->loginLayout->hide();
					if (loginResponse->isActivated()) {
						this->mainLayout->enable();
					}else {
						this->activationLayout->show();
					}
				}else {
					this->loginSaver->clear();
				}
			}
		}

		bool validateLogin(const string& usernameOrEmail, const string& passwordAttempt) {
			if (usernameOrEmail == "") {
				return false;
			}

			if (passwordAttempt == "") {
				return false;
			}

			if (!contains(usernameOrEmail, "@")) {
				string usernameOrEmailWithoutPermittedCharacters = usernameOrEmail;
				erase_all(usernameOrEmailWithoutPermittedCharacters, "_");
				erase_all(usernameOrEmailWithoutPermittedCharacters, ".");
				if (!all(usernameOrEmailWithoutPermittedCharacters, is_alnum())) {
					return false;
				}
			}
			return true;
		}

	};

	typedef SharedPointer<AutomaticLoginHandler> AutomaticLoginHandlerPtr;
}

#endif
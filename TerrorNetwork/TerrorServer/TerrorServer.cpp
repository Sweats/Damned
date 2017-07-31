#include "TerrorServer.h"

namespace terror_network {
	TerrorServer::TerrorServer(ConnectedUserSystemPtr connectedUserSystem) : connectedUserSystem(connectedUserSystem), gameSessionsMutex(new Mutex) {}

	TerrorServer::~TerrorServer() {}

	void TerrorServer::handleTcpPacket(TcpPacketPtr tcpPacket) {
		TcpConnectionPtr connection = this->getTcpConnection();
		int packetLabel = tcpPacket->getLabel();			

		if (packetLabel == GAME_CREATION_REQUEST) {
			GameCreationRequestPtr gameCreationRequest(new GameCreationRequest);
			if (!tcpPacket->buildObject(gameCreationRequest)) {
				return;
			}

			this->connectedUserSystem->lock();
			ConnectedUserPtr user = this->connectedUserSystem->getUser(this->getUuid());
			if (!user || !user->isPartiallyConnected() || !user->isActivated()) {
				this->connectedUserSystem->unlock();
				return;
			}

			this->gameSessionsMutex->try_lock_for(boost::chrono::seconds(60));
			if (this->gameSessionsByUserUuid.contains(this->getUuid())) {
				this->gameSessionsMutex->unlock();
				this->connectedUserSystem->unlock();
				return;
			}
						
			GameSessionPtr gameSession(new GameSession(user, gameCreationRequest->getName(), gameCreationRequest->getPassword(), 
				gameCreationRequest->getNumberOfPrivateSlots(), gameCreationRequest->getMaxPlayers())
				);

			int gameSessionId = this->gameSessions.add(gameSession);
			gameSession->setId(gameSessionId);

			this->gameSessionsByUserUuid[user->getUuid()] = gameSession;

			int nGameSessions = this->gameSessions.size();
			string message = string("Player ") + lexical_cast<string>(connection->getUuid()) + string(" just created game session ") + 
			lexical_cast<string>(gameSessionId) + string("! Number of live game sessions ") + 
			lexical_cast<string>(nGameSessions);
			Logger::getInstance()->logMessage(message);

			this->gameSessionsMutex->unlock();
			this->connectedUserSystem->unlock();

			HostInfoPtr hostInfo(new HostInfo);
			connection->sendObject(hostInfo);			
		}

		if (packetLabel == JOIN_GAME_REQUEST) {
			JoinGameRequestPtr joinGameRequest(new JoinGameRequest);
			if (!tcpPacket->buildObject(joinGameRequest)) {
				return;
			}

			this->connectedUserSystem->lock();
			ConnectedUserPtr user = this->connectedUserSystem->getUser(connection->getUuid());
			if (!user || !user->isPartiallyConnected() || !user->isActivated()) {
				this->connectedUserSystem->unlock();
				string message = string("Host timeout due to client not being partially connected or not activated, host = ") + lexical_cast<string>(user) + string(", partially = ") + lexical_cast<string>(user->isPartiallyConnected());
				Logger::getInstance()->logMessage(message);
				JoinErrorResponsePtr joinErrorResponse(new JoinErrorResponse(JOIN_HOST_TIMEOUT));
				connection->sendObject(joinErrorResponse);
				return;
			}

			this->gameSessionsMutex->try_lock_for(boost::chrono::seconds(60));
			GameSessionPtr gameSessionByUserUuid = this->gameSessionsByUserUuid.get(this->getUuid());
			if (gameSessionByUserUuid) {
				this->gameSessionsMutex->unlock();
				this->connectedUserSystem->unlock();
				if (gameSessionByUserUuid->getId() == joinGameRequest->getId()) {
					Logger::getInstance()->logMessage("Host timeout due to player already in the game session");
				}else {
					Logger::getInstance()->logMessage("Host timeout due to player already in another game session");
					JoinErrorResponsePtr joinErrorResponse(new JoinErrorResponse(JOIN_HOST_TIMEOUT));
					connection->sendObject(joinErrorResponse);
				}
				return;
			}

			GameSessionPtr gameSession = this->gameSessions.get(joinGameRequest->getId());
			if (!gameSession) {
				this->gameSessionsMutex->unlock();
				this->connectedUserSystem->unlock();
				Logger::getInstance()->logMessage("Host timeout due to game session not existing anymore");
				JoinErrorResponsePtr joinErrorResponse(new JoinErrorResponse(JOIN_HOST_TIMEOUT));
				connection->sendObject(joinErrorResponse);
				return;
			}
			if (!gameSession->validatePassword(joinGameRequest->getPassword())) {
				this->gameSessionsMutex->unlock();
				this->connectedUserSystem->unlock();
				JoinErrorResponsePtr joinErrorResponse(new JoinErrorResponse(JOIN_INVALID_PASSWORD));
				connection->sendObject(joinErrorResponse);
				return;
			}
			if (gameSession->isFull()) {
				this->gameSessionsMutex->unlock();
				this->connectedUserSystem->unlock();
				JoinErrorResponsePtr joinErrorResponse(new JoinErrorResponse(JOIN_HOST_FULL));
				connection->sendObject(joinErrorResponse);
				return;
			}			

			ConnectedUserPtr host = gameSession->getHost();
			if (!host || !host->isPartiallyConnected() || !host->isActivated()) {
				this->gameSessionsMutex->unlock();
				this->connectedUserSystem->unlock();
				string message = string("Host timeout due to host not being partially connected, host = ") + lexical_cast<string>(host) + string(", partially = ") + lexical_cast<string>(host->isPartiallyConnected());
				Logger::getInstance()->logMessage(message);
				JoinErrorResponsePtr joinErrorResponse(new JoinErrorResponse(JOIN_HOST_TIMEOUT));
				connection->sendObject(joinErrorResponse);
				return;
			}

			int playerId = gameSession->addPlayer(user);
			this->gameSessionsByUserUuid[user->getUuid()] = gameSession;

			int gameSessionId = gameSession->getId();
			int nPlayers = gameSession->getNumberOfPlayers();
			string message = string("Player ") + lexical_cast<string>(connection->getUuid()) + string(" joined Game Session ") + 
			lexical_cast<string>(gameSessionId) + string("! Players = ") + lexical_cast<string>(nPlayers);
			Logger::getInstance()->logMessage(message);

			this->gameSessionsMutex->unlock();

			JoinInfoPtr joinInfo(new JoinInfo(playerId, host->getUuid()));
			this->sendObject(joinInfo);

			UserInfoPtr userInfo = user->getUserInfo();
			host->sendObject(userInfo);

			this->connectedUserSystem->unlock();			
		}

		if (packetLabel == GAME_SESSION_STAGE_NAME) {
			GameSessionStageNamePtr gameSessionStageName(new GameSessionStageName);
			if (!tcpPacket->buildObject(gameSessionStageName)) {
				return;
			}

			this->gameSessionsMutex->try_lock_for(boost::chrono::seconds(60));
			GameSessionPtr gameSession = this->gameSessionsByUserUuid.get(this->getUuid());
			if (!gameSession) {
				this->gameSessionsMutex->unlock();
				return;
			}
			gameSession->setStageName(gameSessionStageName->getStageName());

			int gameSessionId = gameSession->getId();
			int nPlayers = gameSession->getNumberOfPlayers();
			string message = string("Game session ") + lexical_cast<string>(gameSessionId) + string(" started a new game! Players = ") + lexical_cast<string>(nPlayers);
			this->gameSessionsMutex->unlock();

			Logger::getInstance()->logMessage(message);
		}

		if (packetLabel == LEAVE_GAME_REQUEST) {
			this->leaveGameSession(this->getUuid());
		}

		if (packetLabel == GAME_ENDED) {
			GameEndedPtr gameEnded(new GameEnded);
			if (!tcpPacket->buildObject(gameEnded)) {
				return;
			}
			string message = string("A game just ended, team ") + lexical_cast<string>(gameEnded->getStatus()) + string(" won!");
			Logger::getInstance()->logMessage(message);
		}

		if (packetLabel == GAME_LIST_REQUEST) {
			this->connectedUserSystem->lock();
			ConnectedUserPtr user = this->connectedUserSystem->getUser(this->getUuid());
			if (!user || !user->isPartiallyConnected() || !user->isActivated()) {
				this->connectedUserSystem->unlock();
				return;
			}
			this->connectedUserSystem->unlock();

			this->gameSessionsMutex->try_lock_for(boost::chrono::seconds(60));
			GameSessionListPtr gameSessionList(new GameSessionList(this->gameSessions));
			this->gameSessionsMutex->unlock();
			connection->sendObject(gameSessionList);
		}
	}

	void TerrorServer::handleTcpDisconnect() {
		this->leaveGameSession(this->getUuid());
	}

	void TerrorServer::leaveGameSession(int uuid) {
		this->gameSessionsMutex->try_lock_for(boost::chrono::seconds(60));
		GameSessionPtr gameSession = this->gameSessionsByUserUuid.get(uuid);
		if (!gameSession) {
			this->gameSessionsMutex->unlock();
			return;
		}
		this->gameSessionsByUserUuid.erase(uuid);

		if (gameSession->getNumberOfPlayers() == 1) {
			this->gameSessions.erase(gameSession->getId());
			string message = string("Player ") + lexical_cast<string>(uuid) + string(" just ended game session ") + 
			lexical_cast<string>(gameSession->getId()) + string(", number of live game sessions ") + 
			lexical_cast<string>(this->gameSessions.size());
			Logger::getInstance()->logMessage(message);
		}else {
			gameSession->removePlayer(uuid);
			string message = string("Player ") + lexical_cast<string>(uuid) + string(" just left game session ") + 
			lexical_cast<string>(gameSession->getId()) + string("!");
			Logger::getInstance()->logMessage(message);
		}
		this->gameSessionsMutex->unlock();
	}
}
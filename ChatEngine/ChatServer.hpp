#pragma once

#include "pch.hpp"

#include <GameNetworkingSockets/steam/isteamnetworkingsockets.h>

namespace engine
{
	class ChatServer
	{
	public:
		struct ClientData
		{
			std::wstring nickname;
		};

		ChatServer(uint16_t port);
		~ChatServer();

		ChatServer(const ChatServer&) = delete;
		ChatServer& operator=(const ChatServer&) = delete;
		ChatServer(ChatServer&&) = delete;
		ChatServer& operator=(ChatServer&&) = delete;

		void Start();
		void Stop();

		bool IsRunning() const { return m_running; }
		uint16_t GetPort() const { return m_port; }

	private:
		void NetworkLoop();

		void OnSteamNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t* pInfo );

		void SendStringToClient( HSteamNetConnection conn, const wchar_t* str );
		void SendStringToAllClients( const wchar_t* str, HSteamNetConnection except = k_HSteamNetConnection_Invalid );

		void PollIncomingMessages();
		void PollLocalUserInput();

	private:
		uint16_t m_port = 0;
		bool m_running = false;
		std::thread m_thread;

		ISteamNetworkingSockets* m_pInterface;
		HSteamListenSocket m_hListenSocket;
		HSteamNetPollGroup m_hPollGroup;

		std::unordered_map< HSteamNetConnection, ClientData > clients;
	};
}
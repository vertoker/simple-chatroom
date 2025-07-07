#pragma once

#include "pch.hpp"
#include "ConsoleInput.hpp"

#include <GameNetworkingSockets/steam/isteamnetworkingsockets.h>
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>

namespace engine
{
	class ChatServer
	{
	private:
		struct ClientData_t
		{
			std::wstring nickname;
		};

	public:
		ChatServer(uint16_t port);
		~ChatServer();

		ChatServer(const ChatServer&) = delete;
		ChatServer& operator=(const ChatServer&) = delete;
		ChatServer(ChatServer&&) = delete;
		ChatServer& operator=(ChatServer&&) = delete;

		void Start();
		void Stop();

		inline bool IsRunning() const noexcept { return m_running; }
		inline uint16_t GetPort() const noexcept { return m_port; }

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
		io::ConsoleInput consoleInput;

		ISteamNetworkingSockets* m_pInterface = nullptr;
		HSteamListenSocket m_hListenSocket = k_HSteamListenSocket_Invalid;
		HSteamNetPollGroup m_hPollGroup = k_HSteamNetPollGroup_Invalid;

		std::unordered_map< HSteamNetConnection, ClientData_t > clients;
	};
}
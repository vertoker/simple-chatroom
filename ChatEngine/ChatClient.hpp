#pragma once

#include "pch.hpp"
#include "ConsoleInput.hpp"

#include <GameNetworkingSockets/steam/isteamnetworkingsockets.h>
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>

namespace engine
{
	class ChatClient
	{

	public:
		ChatClient(const SteamNetworkingIPAddr& address);
		~ChatClient();

		ChatClient(const ChatClient&) = delete;
		ChatClient& operator=(const ChatClient&) = delete;
		ChatClient(ChatClient&&) = delete;
		ChatClient& operator=(ChatClient&&) = delete;

		void Start();
		void Stop();

		bool IsRunning() const { return m_running; }
		uint16_t GetPort() const { return m_port; }

	private:
		SteamNetworkingIPAddr m_address; // TODO
		bool m_running = false;
		std::thread m_thread;
		io::ConsoleInput consoleInput;

		ISteamNetworkingSockets* m_pInterface = nullptr;
		HSteamNetConnection m_hConnection = k_HSteamNetConnection_Invalid;
	};
}
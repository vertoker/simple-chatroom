#ifndef __CHAT_CLIENT_HPP_
#define __CHAT_CLIENT_HPP_

#include "pch.hpp"
#include "ConsoleInput.hpp"
#include "Serialization.hpp"

#include <GameNetworkingSockets/steam/isteamnetworkingsockets.h>
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>

namespace engine
{
	class ChatClient
	{

	public:
		ChatClient(const engine::Config& config);
		~ChatClient();

		ChatClient(const ChatClient&) = delete;
		ChatClient& operator=(const ChatClient&) = delete;
		ChatClient(ChatClient&&) = delete;
		ChatClient& operator=(ChatClient&&) = delete;

		void Start();
		void Stop();

		inline bool IsRunning() const noexcept { return m_running; }
		inline SteamNetworkingIPAddr GetAddress() const noexcept { return m_serverAddress; }

	private:
		void NetworkLoop();

		void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo);

		void PollIncomingMessages();
		void PollLocalUserInput();

	private:
		SteamNetworkingIPAddr m_serverAddress;
		bool m_running = false;
		std::thread m_thread;
		io::ConsoleInput consoleInput;

		ISteamNetworkingSockets* m_pInterface = nullptr;
		HSteamNetConnection m_hConnection = k_HSteamNetConnection_Invalid;
	};
}
#endif // !__CHAT_CLIENT_HPP_
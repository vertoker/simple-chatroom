#ifndef __CHAT_SERVER_HPP_
#define __CHAT_SERVER_HPP_

#include "pch.hpp"
#include "ConsoleInput.hpp"
#include "Serialization.hpp"

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
		ChatServer(const engine::Config& config);
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
#endif // !__CHAT_SERVER_HPP_
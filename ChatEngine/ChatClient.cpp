
#include "ChatClient.hpp"
#include "Logger.hpp"
#include "Utilities.hpp"

#include <functional>

#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>

engine::ChatClient::ChatClient(const SteamNetworkingIPAddr& address) : m_serverAddress{address}
{

}
engine::ChatClient::~ChatClient()
{
	if (m_thread.joinable())
		m_thread.join();
}

void engine::ChatClient::Start()
{
	if (m_running) return;
	m_running = true;

	consoleInput.Start();
	m_thread = std::thread([this]() { NetworkLoop(); });
}
void engine::ChatClient::Stop()
{
	m_running = false;
	consoleInput.Stop();
}

void engine::ChatClient::NetworkLoop()
{
	SteamDatagramErrMsg errMsg;
	if (!GameNetworkingSockets_Init(nullptr, errMsg))
	{
		io::werror() << "GameNetworkingSockets_Init failed: " << errMsg;
		return;
	}

	m_pInterface = SteamNetworkingSockets();

	SteamNetworkingConfigValue_t options{};
	auto lambda = [this](SteamNetConnectionStatusChangedCallback_t* pInfo) { this->OnSteamNetConnectionStatusChanged(pInfo); };
	std::function<void(SteamNetConnectionStatusChangedCallback_t*)> fn(lambda);
	options.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, &lambda); // TODO make static

	m_hConnection = m_pInterface->ConnectByIPAddress(m_serverAddress, 1, &options);
	if (m_hConnection == k_HSteamNetConnection_Invalid)
	{
		io::werror() << "Failed to create connection";
		Stop();
		return;
	}

	io::winfo() << "Start client";

	char szAddress[SteamNetworkingIPAddr::k_cchMaxString];
	m_serverAddress.ToString(szAddress, sizeof(szAddress), true);
	io::winfo() << "Connnected to chat server at " << szAddress;

	while (m_running)
	{
		PollIncomingMessages();
		PollLocalUserInput();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	io::winfo() << "Stop client";
}


void engine::ChatClient::OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
	if (pInfo->m_hConn != m_hConnection && m_hConnection != k_HSteamNetConnection_Invalid)
	{
		io::werror() << "Wrong connection, can't use it, drop";
		Stop();
		return;
	}

	switch (pInfo->m_info.m_eState)
	{
	case k_ESteamNetworkingConnectionState_None:
	{
		// We will get callbacks here when we destroy connections. You can ignore these
		break;
	}
	case k_ESteamNetworkingConnectionState_ClosedByPeer:
	case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
	{
		Stop();

		if (pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connecting)
		{
			io::winfo() << "Host reject you. " << pInfo->m_info.m_szEndDebug;
		}
		else if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
		{
			io::winfo() << "Can't connect to host. " << pInfo->m_info.m_szEndDebug;
		}
		else
		{
			io::winfo() << "Unknown disconnect. " << pInfo->m_info.m_szEndDebug;
		}

		// Clean up connection, even it's already closed externally
		m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
		m_hConnection = k_HSteamNetConnection_Invalid;
		break;
	}
	case k_ESteamNetworkingConnectionState_Connecting:
	{
		// When start connection
		break;
	}
	case k_ESteamNetworkingConnectionState_Connected:
	{
		io::winfo() << "Connected to server";
		break;
	}
	default:
	{
		break;
	}
	}
}

void engine::ChatClient::PollIncomingMessages()
{
	//wchar_t buffer[4096];
	while (m_running)
	{
		ISteamNetworkingMessage* pIncomingMessage = nullptr;
		int numMessages = m_pInterface->ReceiveMessagesOnConnection(m_hConnection, &pIncomingMessage, 1);
		if (numMessages == 0) break; // no messages

		if (numMessages < 0) // -1
		{
			io::werror() << "The connection handle is invalid, stop server";
			Stop();
			return;
		}
		if (numMessages > 1)
		{
			io::werror() << "numMessages (" << numMessages << ") bigger than 1";
			return;
		}
		if (!pIncomingMessage)
		{
			io::werror() << "Invalid pIncomingMessage";
			return;
		}

		std::wstring bufferCmd;
		bufferCmd.assign((const wchar_t*)pIncomingMessage->m_pData, pIncomingMessage->m_cbSize / sizeof(wchar_t));

		pIncomingMessage->Release();

		// Next do something with *cmd

		io::wprint() << bufferCmd;
	}
}

void engine::ChatClient::PollLocalUserInput()
{
	std::wstring cmd;
	while (m_running && consoleInput.GetNext(cmd))
	{
		if (wcscmp(cmd.c_str(), L"/quit") == 0)
		{
			io::winfo(L"Disconnect from chat server");
			Stop();
			m_pInterface->CloseConnection(m_hConnection, 0, "Goodbye", true);
			break;
		}

		m_pInterface->SendMessageToConnection(m_hConnection, cmd.c_str(), (uint32_t)cmd.length() / sizeof(wchar_t), k_nSteamNetworkingSend_Reliable, nullptr); // TODO
		io::winfo(L"The server only knows one command: '/quit'");
	}
}
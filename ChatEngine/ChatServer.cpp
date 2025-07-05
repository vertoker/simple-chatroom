
#include "pch.hpp"
#include "ChatServer.hpp"
#include "Logger.hpp"

engine::ChatServer::ChatServer(uint16_t port) : m_port(port)
{

}
engine::ChatServer::~ChatServer()
{
	if (m_thread.joinable())
		m_thread.join();
}

void engine::ChatServer::Start()
{
	if (m_running)
		return;

	m_thread = std::thread([this]() { NetworkLoop(); });
}
void engine::ChatServer::Stop()
{
	m_running = false;
}

void engine::ChatServer::NetworkLoop()
{
	m_running = false;

	m_pInterface = SteamNetworkingSockets();

	SteamNetworkingIPAddr serverLocalAddress{};
	serverLocalAddress.Clear();
	serverLocalAddress.m_port = m_port;

	SteamNetworkingConfigValue_t options{};
	auto lambda = [this](SteamNetConnectionStatusChangedCallback_t* pInfo) { this->OnSteamNetConnectionStatusChanged(pInfo); };
	std::function<void(SteamNetConnectionStatusChangedCallback_t*)> fn(lambda);
	options.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, &fn ); // TODO make static

	m_hListenSocket = m_pInterface->CreateListenSocketIP( serverLocalAddress, 1, &options );
	if (m_hListenSocket == k_HSteamListenSocket_Invalid)
	{
		debug::werror() << "Failed to listen on port " << m_port;
		return;
	}

	m_hPollGroup = m_pInterface->CreatePollGroup();
	if (m_hPollGroup == k_HSteamNetPollGroup_Invalid)
	{
		debug::werror() << "Failed to listen on port " << m_port;
		return;
	}

	debug::winfo() << "Start server";


	while (m_running)
	{
		PollIncomingMessages();
		PollLocalUserInput();
		std::this_thread::sleep_for( std::chrono::milliseconds(10) );
	}
	

	debug::winfo() << "Closing connections...";
	
	for (const auto& pair : clients)
	{
		SendStringToClient(pair.first, L"Server is shotting down. Goodbye...");
		m_pInterface->CloseConnection(pair.first, 0, "Server Shutdown", true);
	}

	clients.clear();

	m_pInterface->CloseListenSocket( m_hListenSocket );
	m_hListenSocket = k_HSteamListenSocket_Invalid;
	m_pInterface->DestroyPollGroup( m_hPollGroup );
	m_hPollGroup = k_HSteamNetPollGroup_Invalid;
}

void engine::ChatServer::OnSteamNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t* pInfo )
{

}


void engine::ChatServer::SendStringToClient( HSteamNetConnection conn, const wchar_t* str )
{
	m_pInterface->SendMessageToConnection(conn, str, (uint32)wcslen(str), k_nSteamNetworkingSend_Reliable, nullptr);
}
void engine::ChatServer::SendStringToAllClients( const wchar_t* str, HSteamNetConnection except )
{
	for (auto& pair : clients)
	{
		if (pair.first != except)
			SendStringToClient(pair.first, str);
	}
}

void engine::ChatServer::PollIncomingMessages()
{
	const int maxMessages = 1;
	wchar_t buffer[4096];

	while (m_running)
	{
		ISteamNetworkingMessage* pIncomingMessage = nullptr;
		int numMessages = m_pInterface->ReceiveMessagesOnConnection( m_hPollGroup, &pIncomingMessage, maxMessages);
		if (numMessages == 0) break;
		
		if (numMessages < 0 || numMessages > maxMessages || !pIncomingMessage)
		{
			debug::werror() << "Error while poll incoming messages";
			return;
		}

		auto itClient = clients.find( pIncomingMessage->m_conn );
		if (itClient == clients.end())
		{
			debug::werror() << "Can't find client of message";
			return;
		}

		std::wstring bufferCmd;
		bufferCmd.assign( (const wchar_t*) pIncomingMessage->m_pData, pIncomingMessage->m_cbSize / sizeof(wchar_t) );

		pIncomingMessage->Release();

		// Next do something with *cmd
		
		// /nick 5

		swprintf_s(buffer, L"[%s] %s", itClient->second.nickname.c_str(), bufferCmd.c_str());

		debug::wprint() << buffer;
		SendStringToAllClients(buffer, itClient->first);
	}
	m_pInterface->RunCallbacks();
}
void engine::ChatServer::PollLocalUserInput()
{
	std::wstring cmd;
	while (m_running && LocalUserInput_GetNext(cmd))
	{
		if (wstrcmp(cmd.c_str(), "/quit") == 0)
		{
			g_bQuit = true;
			Printf("Shutting down server");
			break;
		}

		// That's the only command we support
		Printf("The server only knows one command: '/quit'");
	}
}

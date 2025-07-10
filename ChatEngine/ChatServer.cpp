
#include "ChatServer.hpp"
#include "Logger.hpp"
#include "Utilities.hpp"

#include <functional>

#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include <GameNetworkingSockets/steam/isteamnetworkingutils.h>
#ifndef STEAMNETWORKINGSOCKETS_OPENSOURCE
#include <GameNetworkingSockets/steam/steam_api.h>
#endif

static engine::ChatServer* s_pCallbackInstance;

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
	if (m_running) return;
	m_running = true;

	consoleInput.Start();
	m_thread = std::thread([this]() { NetworkLoop(); });
}
void engine::ChatServer::Stop()
{
	m_running = false;
	consoleInput.Stop();
}

void engine::ChatServer::NetworkLoop()
{
	SteamDatagramErrMsg errMsg;
	if (!GameNetworkingSockets_Init(nullptr, errMsg))
	{
		io::werror() << "GameNetworkingSockets_Init failed: " << errMsg;
		return;
	}

	m_pInterface = SteamNetworkingSockets();

	SteamNetworkingIPAddr serverLocalAddress{};
	serverLocalAddress.Clear();
	serverLocalAddress.m_port = m_port;

	SteamNetworkingConfigValue_t options{};
	options.SetPtr( k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)SteamNetConnectionStatusChangedCallback );

	m_hListenSocket = m_pInterface->CreateListenSocketIP( serverLocalAddress, 1, &options );
	if (m_hListenSocket == k_HSteamListenSocket_Invalid)
	{
		io::werror() << "Failed to listen on port " << m_port << " (listen socket)";
		return;
	}

	m_hPollGroup = m_pInterface->CreatePollGroup();
	if (m_hPollGroup == k_HSteamNetPollGroup_Invalid)
	{
		io::werror() << "Failed to listen on port " << m_port << " (poll group)";
		return;
	}

	io::winfo() << "Listening on port " << m_port;

	io::winfo() << "Start server";

	while (m_running)
	{
		PollIncomingMessages();
		PollConnectionStateChanges();
		PollLocalUserInput();
		std::this_thread::sleep_for( std::chrono::milliseconds(10) );
	}
	
	io::winfo() << "Stop server";
	
	for (const auto& pair : clients)
	{
		SendStringToClient(pair.first, L"Server is shutting down. Goodbye...");
		m_pInterface->CloseConnection(pair.first, 0, "Server Shutdown", true);
	}

	clients.clear();

	m_pInterface->CloseListenSocket( m_hListenSocket );
	m_hListenSocket = k_HSteamListenSocket_Invalid;
	m_pInterface->DestroyPollGroup( m_hPollGroup );
	m_hPollGroup = k_HSteamNetPollGroup_Invalid;

	m_pInterface = nullptr;
	GameNetworkingSockets_Kill();
}

void engine::ChatServer::OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo)  
{
    wchar_t buffer[4096];

    switch (auto state = pInfo->m_info.m_eState; state)
    {
		case k_ESteamNetworkingConnectionState_None:
		{
		    // callbacks if connections have been destroyed
		    break;
		}
		case k_ESteamNetworkingConnectionState_ClosedByPeer:
		case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
		{
			if (pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connected)
			{
				auto itClient = clients.find(pInfo->m_hConn);
				if (itClient == clients.end())
				{
					io::werror() << "Can't find client by conn " << pInfo->m_info.m_szConnectionDescription;
					Stop();
					return;
				}

				const wchar_t* debugLogAction;
				if (state == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
				{
					debugLogAction = L"problem detected locally";
					swprintf_s(buffer, L"Alas, %s hath fallen into shadow.  (%hs)", itClient->second.nickname.c_str(), pInfo->m_info.m_szEndDebug);
				}
				else
				{
					debugLogAction = L"closed by peer";
					swprintf_s(buffer, L"%s hath departed", itClient->second.nickname.c_str());
				}

				io::winfo() << L"Connection " << pInfo->m_info.m_szConnectionDescription
					<< ' ' << debugLogAction
					<< L", reason " << pInfo->m_info.m_eEndReason
					<< L": " << pInfo->m_info.m_szEndDebug;

				clients.erase(itClient);

				SendStringToAllClients(buffer);
			}
			else if (pInfo->m_eOldState != k_ESteamNetworkingConnectionState_Connecting)
			{
				io::werror() << "Failed to connect, previous state is k_ESteamNetworkingConnectionState_Connecting";
				Stop();
				return;
			}

			// Must clean up connection even if it's closed, data isn't important (0's)
			m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
			break;
		}
		case k_ESteamNetworkingConnectionState_Connecting:
		{
			if (clients.find(pInfo->m_hConn) != clients.end())
			{
				io::werror() << "Found connection " << pInfo->m_info.m_szConnectionDescription << ", this must be a new connection";
				Stop();
				return;
			}
			io::winfo() << "Connection request from " << pInfo->m_info.m_szConnectionDescription;

			// Try to accept connection
			if (m_pInterface->AcceptConnection(pInfo->m_hConn) != k_EResultOK)
			{
				// Even connection internally could be failed
				m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
				io::wwarning() << "Can't accept connection. (It was already closed?)";
				break;
			}

			// Assign the pool group
			if (!m_pInterface->SetConnectionPollGroup(pInfo->m_hConn, m_hPollGroup))
			{
				m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
				io::wwarning() << "Failed to set pool group";
			}

			auto rndNickname = utility::GenerateNickname();
			swprintf_s(buffer, L"Welcome in chat, '%s'", rndNickname.c_str());
			SendStringToClient(pInfo->m_hConn, buffer);

			if (clients.empty())
			{
				SendStringToClient(pInfo->m_hConn, L"Right now you are here alone");
			}
			else
			{
				swprintf_s(buffer, L"%s companions with you", clients.size());
				SendStringToClient(pInfo->m_hConn, buffer);
				swprintf_s(buffer, L"%s joined in chat", rndNickname.c_str());
				SendStringToAllClients(buffer, pInfo->m_hConn);
			}

			clients[pInfo->m_hConn]; // cool
			clients[pInfo->m_hConn].nickname = std::move(rndNickname);

			break;
		}
		case k_ESteamNetworkingConnectionState_Connected:
		{
			// Client already connected, do nothing
			break;
		}
		default:
		{
			break;
		}
    }
}
void engine::ChatServer::SteamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
	s_pCallbackInstance->OnSteamNetConnectionStatusChanged(pInfo);
}

void engine::ChatServer::SendStringToClient( HSteamNetConnection conn, const wchar_t* str )
{
	m_pInterface->SendMessageToConnection(conn, str, (uint32)(wcslen(str) * sizeof(wchar_t)), k_nSteamNetworkingSend_Reliable, nullptr);
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
	wchar_t buffer[4096];

	if (clients.size() == 0) return; // no clients

	while (m_running)
	{
		ISteamNetworkingMessage* pIncomingMessage = nullptr;
		int numMessages = m_pInterface->ReceiveMessagesOnConnection( m_hPollGroup, &pIncomingMessage, 1);
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

		auto itClient = clients.find( pIncomingMessage->m_conn );
		if (itClient == clients.end())
		{
			io::werror() << "Received data from unhandled client";
			return;
		}

		std::wstring bufferCmd;
		bufferCmd.assign( (const wchar_t*) pIncomingMessage->m_pData, pIncomingMessage->m_cbSize / sizeof(wchar_t) );

		pIncomingMessage->Release();

		// Next do something with *cmd
		
		// /nick 5

		swprintf_s(buffer, L"[%s] %s", itClient->second.nickname.c_str(), bufferCmd.c_str());

		io::wprint() << buffer;
		SendStringToAllClients(buffer, itClient->first);
	}
}
void engine::ChatServer::PollConnectionStateChanges()
{
	s_pCallbackInstance = this;
	m_pInterface->RunCallbacks();
}
void engine::ChatServer::PollLocalUserInput()
{
	std::wstring cmd;
	while (m_running && consoleInput.GetNext(cmd))
	{
		if (wcscmp(cmd.c_str(), L"/quit") == 0)
		{
			io::winfo(L"Shutting down server");
			Stop();
			break;
		}

		io::winfo(L"The server only knows one command: '/quit'");
	}
}

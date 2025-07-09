#pragma once

#include <string>

#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include <GameNetworkingSockets/steam/isteamnetworkingutils.h>
#ifndef STEAMNETWORKINGSOCKETS_OPENSOURCE
#include <GameNetworkingSockets/steam/steam_api.h>
#endif

namespace engine
{
	struct Config
	{
		bool IsServer;
		SteamNetworkingIPAddr Address;

		void PrintData();

		static Config GetDefaultServer();
		static Config GetDefaultClient();
		static std::wstring GetDefaultPath(const std::wstring& name = L"/config.json");

		static bool Save(const Config& config, const std::wstring& path);
		static bool Load(Config& config, const std::wstring& path);
	};
}
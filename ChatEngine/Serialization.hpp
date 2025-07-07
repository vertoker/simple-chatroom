#ifndef __SERIALIZATION_HPP_
#define __SERIALIZATION_HPP_

#include "pch.hpp"

#include <GameNetworkingSockets/steam/isteamnetworkingsockets.h>
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>

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
#endif // !__SERIALIZATION_HPP_

#include "Serialization.hpp"
#include "Logger.hpp"

#include <sstream>
#include <string>

#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include <GameNetworkingSockets/steam/isteamnetworkingutils.h>
#ifndef STEAMNETWORKINGSOCKETS_OPENSOURCE
#include <GameNetworkingSockets/steam/steam_api.h>
#endif
#include "nlohmann/json.hpp"
#include <boost/dll.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;
static const uint16_t defaultPort = 27020;

void engine::Config::PrintData()
{
	char bufferIP[SteamNetworkingIPAddr::k_cchMaxString];
	Address.ToString(bufferIP, sizeof(bufferIP), true);

	io::winfo() << "IsServer = " << (IsServer ? "true" : "false");
	io::winfo() << "Address = " << bufferIP;
}

engine::Config engine::Config::GetDefaultServer()
{
	Config config{};
	config.IsServer = true;
	config.Address.SetIPv4(0, defaultPort);
	return config;
}
engine::Config engine::Config::GetDefaultClient()
{
	Config config{};
	config.IsServer = false;
	config.Address.SetIPv4(0, defaultPort);
	return config;
}
std::wstring engine::Config::GetDefaultPath(const std::wstring& name)
{
	auto programLocation = boost::dll::program_location().parent_path();// .generic_path();
	std::wostringstream woss;
	woss << programLocation.c_str() << name;
	return woss.str();
}

bool engine::Config::Save(const Config& config, const std::wstring& path)
{
	json json;
	json["IsServer"] = config.IsServer;
	char bufferIP[SteamNetworkingIPAddr::k_cchMaxString];
	config.Address.ToString(bufferIP, sizeof(bufferIP), true);
	std::string addressStr(bufferIP);
	json["Address"] = addressStr;

	std::ofstream ofs;
	ofs.open(path, std::ios_base::trunc);

	if (!ofs.is_open())
	{
		io::wwarning() << "Can't open (to save) config file by path " << path;
		return false;
	}

	ofs << json.dump(4).c_str();

	return true;
}
bool engine::Config::Load(Config& config, const std::wstring& path)
{
	std::ifstream ifs;
	ifs.open(path);

	if (!ifs.is_open())
	{
		io::wwarning() << "Can't open (to load) config file by path " << path;
		return false;
	}

	json json = json::parse(ifs);
	config.IsServer = json["IsServer"];
	std::string addressStr = json["Address"];
	config.Address.ParseString(addressStr.c_str());

	return true;
}

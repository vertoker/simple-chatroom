#include "pch.hpp"
#include "Serialization.hpp"
#include "Logger.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

engine::Config engine::Config::GetDefaultServer()
{
	Config config{};
	config.IsServer = true;
	return config;
}
engine::Config engine::Config::GetDefaultClient()
{
	Config config{};
	config.IsServer = false;
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

	return true;
}

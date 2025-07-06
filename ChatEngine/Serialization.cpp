#include "pch.hpp"
#include "Serialization.hpp"
#include "Logger.hpp"

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

using json = nlohmann::json;

void engine::Config::Save(const Config& config, const std::wstring& path)
{
	std::ifstream ifs;
	ifs.open(path);

	if (!ifs.is_open())
	{
		io::werror() << "Can't open config file by path " << path;
		return;
	}

	json json = json::parse(ifs);
}
/*engine::Config engine::Config::Load(const std::string& path)
{
	Config config{};
	return config; // TODO
}*/

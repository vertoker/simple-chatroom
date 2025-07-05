#pragma once

#include "pch.hpp"

namespace engine
{
	struct Config
	{
		bool IsServer;

		static Config GetDefaultServer();
		static Config GetDefaultClient();

		static void Save(const Config& config, const std::string& path = "./config.json");
		static Config Load(const std::string& path = "./config.json");
	};
}
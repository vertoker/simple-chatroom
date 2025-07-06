#pragma once

#include "pch.hpp"

namespace engine
{
	struct Config
	{
		bool IsServer;

		static Config GetDefaultServer();
		static Config GetDefaultClient();
		static std::wstring GetDefaultPath(const std::wstring& name = L"/config.json");

		static bool Save(const Config& config, const std::wstring& path);
		static bool Load(Config& config, const std::wstring& path);
	};
}
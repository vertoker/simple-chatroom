#include "pch.hpp"
#include "Logger.hpp"

void debug::log(LogType type, const std::string& message)
{
	std::stringstream ss;
    switch (type)
    {
    case LogType::Info:
        ss << "[I]: ";
        break;
    case LogType::Warning:
        ss << "[W]: ";
        break;
    case LogType::Error:
        ss << "[E]: ";
        break;
    case LogType::Debug:
#ifdef NDEBUG
        return;
#endif
        ss << "[D]: ";
        break;
    }

    ss << message;
    auto str = ss.str();
    std::cout << str << std::endl;
}

void debug::log(LogType type, const std::wstring& message)
{
	std::wstringstream wss;
    switch (type)
    {
    case LogType::Info:
        wss << "[I]: ";
        break;
    case LogType::Warning:
        wss << "[W]: ";
        break;
    case LogType::Error:
        wss << "[E]: ";
        break;
    case LogType::Debug:
#ifdef NDEBUG
        return;
#endif
        wss << "[D]: ";
        break;
    }

    wss << message;
    auto wstr = wss.str();
    std::wcout << wstr << std::endl;
}

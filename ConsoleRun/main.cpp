#include "ChatEngine/ChatServer.hpp"
#include "ChatEngine/Serialization.hpp"
#include "ChatEngine/Logger.hpp"
//#include "ChatEngine/ConsoleInput.hpp"

#include <io.h>
#include <fcntl.h>

void enableUTF16InWinConsole()
{
    // https://ru.stackoverflow.com/questions/459154/Русский-язык-в-консоли
    if (_setmode(_fileno(stdout), _O_U16TEXT) == -1)
        io::error("Failed to set mode for stdout to UTF-16");
    if (_setmode(_fileno(stdin), _O_U16TEXT) == -1)
        io::error("Failed to set mode for stdin to UTF-16");
    if (_setmode(_fileno(stderr), _O_U16TEXT) == -1)
        io::error("Failed to set mode for stderr to UTF-16");
}

int main()
{
    enableUTF16InWinConsole();

    /*engine::ConsoleInput consoleInput;
    consoleInput.Start();
    std::wstring buffer;
    while (!consoleInput.GetNext(buffer));
    debug::winfo() << buffer;
    consoleInput.Stop();*/

    //wchar_t buffer2[1024];
    //swprintf_s(buffer2, L"[%s] %s", L"123", L"Костя");
    //debug::wprint() << buffer2;

    engine::Config config;
    auto path = engine::Config::GetDefaultPath();
    if (engine::Config::Load(config, path))
    {
        io::winfo() << "Load config, IsServer = " << (config.IsServer ? "true" : "false");
    }
    else
    {
        config = engine::Config::GetDefaultClient();
        engine::Config::Save(config, path);
        io::winfo() << "Create default config, IsServer = " << (config.IsServer ? "true" : "false");
    }

    if (config.IsServer)
    {
        engine::ChatServer server(37345);

        server.Start();
        while (server.IsRunning());
        server.Stop();
    }
    else
    {
        io::wwarning() << "No client support yet";
    }

	io::wprint() << "Press any key to exit...";
	std::wcin.ignore();

	return 0;
}
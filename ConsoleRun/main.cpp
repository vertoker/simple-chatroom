#include "ChatEngine/ChatServer.hpp"
#include "ChatEngine/Serialization.hpp"
#include "ChatEngine/Logger.hpp"

#include <io.h>
#include <fcntl.h>

void enableUTF16InWinConsole()
{
    // https://ru.stackoverflow.com/questions/459154/Русский-язык-в-консоли
    if (_setmode(_fileno(stdout), _O_U16TEXT) == -1)
        debug::error("Failed to set mode for stdout to UTF-16");
    if (_setmode(_fileno(stdin), _O_U16TEXT) == -1)
        debug::error("Failed to set mode for stdin to UTF-16");
    if (_setmode(_fileno(stderr), _O_U16TEXT) == -1)
        debug::error("Failed to set mode for stderr to UTF-16");
}

int main()
{
    enableUTF16InWinConsole();

    char buffer[1024];
    sprintf_s(buffer, "[%s] %s", "123", "321");
    debug::wprint() << buffer;

    wchar_t buffer2[1024];
    swprintf_s(buffer2, L"[%s] %s", L"123", L"Костя");
    debug::wprint() << buffer2;

	//engine::ChatServer server(37345);

	debug::wprint() << "Press any key to exit...";
	std::wcin.ignore();

	return 0;
}
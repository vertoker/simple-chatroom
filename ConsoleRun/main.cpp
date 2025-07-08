#include "ChatEngine/Engine.hpp"

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

    engine::Engine engine;
    engine.Run();

	return 0;
}
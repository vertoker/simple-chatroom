#include "Engine.hpp"

engine::Engine::Engine()
{

}
engine::Engine::~Engine()
{

}

void engine::Engine::Run()
{
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
    /*if (engine::Config::Load(config, path))
    {
        io::winfo() << "Load config";
        config.PrintData();
    }
    else
    {
        config = engine::Config::GetDefaultClient();
        engine::Config::Save(config, path);
        io::winfo() << "Create default config";
        config.PrintData();
    }*/

    engine::ChatServer server(config);
    /*if (config.IsServer)
    {
        engine::ChatServer server(config.Address.m_port);

        server.Start();
        while (server.IsRunning());
        server.Stop();
    }
    else
    {
        engine::ChatClient client(config.Address);

        client.Start();
        while (client.IsRunning());
        client.Stop();
    }

    io::wprint() << "Press any key to exit...";
    std::wcin.ignore();*/
}

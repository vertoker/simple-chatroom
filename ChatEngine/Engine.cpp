
#include "Engine.hpp"

#include <iostream>

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

    auto path = engine::Config::GetDefaultPath();
    engine::Config config;
    if (engine::Config::Load(config, path))
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
    }

    //engine::ChatServer server(36363);
    if (config.IsServer)
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
    std::wcin.ignore();
}

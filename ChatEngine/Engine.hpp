#ifndef __ENGINE_HPP_
#define __ENGINE_HPP_

#include "pch.hpp"
#include "Utilities.hpp"
#include "Logger.hpp"
#include "ConsoleInput.hpp"

#include "Serialization.hpp"
#include "ChatServer.hpp"
#include "ChatClient.hpp"

namespace engine
{
	class Engine
	{
	public:
		Engine();
		~Engine();
		
		void Run();
	};
}

#endif // !__ENGINE_HPP_

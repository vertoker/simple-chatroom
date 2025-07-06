#pragma once

#include "pch.hpp"

namespace io
{
	class ConsoleInput
	{
	public:
		ConsoleInput();
		~ConsoleInput();

		void Start();
		void Stop();

		bool GetNext(std::wstring& result);

	private:
		void InputLoop();

	private:
		std::mutex m_userInputMutex;
		std::queue<std::wstring> m_queueUserInput;
		std::thread m_thread;
		bool m_running = false;
	};
}
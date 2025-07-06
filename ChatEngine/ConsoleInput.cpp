
#include "pch.hpp"
#include "ConsoleInput.hpp"
#include "Utilities.hpp"
#include "Logger.hpp"

engine::ConsoleInput::ConsoleInput()
{

}
engine::ConsoleInput::~ConsoleInput()
{
	if (m_thread.joinable())
		m_thread.join();
}

void engine::ConsoleInput::Start()
{
	if (m_running) return;

	m_thread = std::thread([this]() { InputLoop(); });
}
void engine::ConsoleInput::Stop()
{
	m_running = false;
}

void engine::ConsoleInput::InputLoop()
{
	while (m_running)
	{
		wchar_t szLine[4000];
		if (!fgetws(szLine, sizeof(szLine) / sizeof(wchar_t), stdin)) // TODO potencial error
		{
			// This thread can be closed only from the inside
			if (!m_running) return;
			Stop();
			debug::werror(L"Failed to read on stdin, quitting");
			break;
		}

		m_userInputQueue.lock();
		m_queueUserInput.push(std::wstring(szLine));
		m_userInputQueue.unlock();
	}
}

bool engine::ConsoleInput::GetNext( std::wstring& result )
{
	bool got_input = false;
	m_userInputQueue.lock();
	while (!m_queueUserInput.empty() && !got_input)
	{
		result = m_queueUserInput.front();
		m_queueUserInput.pop();
		utility::ltrim(result);
		utility::rtrim(result);
		got_input = !result.empty(); // ignore blank lines
	}
	m_userInputQueue.unlock();
	return got_input;
}

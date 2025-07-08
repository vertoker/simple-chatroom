
#include "ConsoleInput.hpp"
#include "Utilities.hpp"
#include "Logger.hpp"

#include <iostream>

io::ConsoleInput::ConsoleInput()
{

}
io::ConsoleInput::~ConsoleInput()
{
	if (m_thread.joinable())
		m_thread.join();
}

void io::ConsoleInput::Start()
{
	if (m_running) return;
	m_running = true;

	m_thread = std::thread([this]() { InputLoop(); });
}
void io::ConsoleInput::Stop()
{
	m_running = false;

	//io::info(L"close stdin"); // TODO can't cancel stdin
	//_close(_fileno(stdin));
	//std::wcin.eof();
}

void io::ConsoleInput::InputLoop()
{
	while (m_running)
	{
		wchar_t szLine[4096];
		if (!(std::wcin >> szLine))
		//if (!fgetws(szLine, sizeof(szLine) / sizeof(wchar_t), stdin)) // TODO potencial error
		{
			// This thread can be closed only from the inside
			if (!m_running) return;
			Stop();
			io::werror(L"Failed to read on stdin, quitting");
			break;
		}

		m_userInputMutex.lock();
		m_queueUserInput.push(std::wstring(szLine));
		m_userInputMutex.unlock();
	}
}

bool io::ConsoleInput::GetNext( std::wstring& result )
{
	bool got_input = false;
	m_userInputMutex.lock();
	while (m_running && !m_queueUserInput.empty() && !got_input)
	{
		result = m_queueUserInput.front();
		m_queueUserInput.pop();
		utility::ltrim(result);
		utility::rtrim(result);
		got_input = !result.empty(); // ignore blank lines
	}
	m_userInputMutex.unlock();
	return got_input;
}

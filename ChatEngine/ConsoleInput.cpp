
#include "pch.hpp"
#include "ConsoleInput.hpp"

bool engine::ConsoleInput::LocalUserInput_GetNext( std::wstring& result )
{
	bool got_input = false;
	mutexUserInputQueue.lock();
	while (!queueUserInput.empty() && !got_input)
	{
		result = queueUserInput.front();
		queueUserInput.pop();
		ltrim(result);
		rtrim(result);
		got_input = !result.empty(); // ignore blank lines
	}
	mutexUserInputQueue.unlock();
	return got_input;
}

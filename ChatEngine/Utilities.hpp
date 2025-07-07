#ifndef __UTILITIES_HPP_
#define __UTILITIES_HPP_

#include "pch.hpp"

namespace utility
{
	// trim from start (in place)
	template <class _Elem>
	inline void ltrim(std::basic_string<_Elem, std::char_traits<_Elem>, std::allocator<_Elem>>& s)
	{
		s.erase(s.begin(), std::find_if(s.begin(), s.end(),
			[](int ch) { return !std::isspace(ch); }
		));
	}

	// trim from end (in place)
	template <class _Elem>
	inline void rtrim(std::basic_string<_Elem, std::char_traits<_Elem>, std::allocator<_Elem>>& s)
	{
		s.erase(std::find_if(s.rbegin(), s.rend(),
			[](int ch) { return !std::isspace(ch); }
		).base(), s.end());
	}

	std::wstring GenerateNickname();
}
#endif // !__UTILITIES_HPP_
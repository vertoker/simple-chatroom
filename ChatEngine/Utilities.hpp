#pragma once

#include <string>

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

	template <class _Elem>
	class StringBuffer
	{
	public:
		typedef std::basic_string<_Elem, std::char_traits<_Elem>, std::allocator<_Elem>> string_type;
		typedef std::basic_ostringstream<_Elem, std::char_traits<_Elem>, std::allocator<_Elem>> ostringstream_type;

	public:
		StringBuffer() : m_str{}, m_oss{ m_str } { }
		~StringBuffer() { }

		StringBuffer(const StringBuffer&) = default;
		StringBuffer(StringBuffer&&) = default;
		StringBuffer& operator=(const StringBuffer&) = default;
		StringBuffer& operator=(StringBuffer&&) = default;

		inline const string_type&        str() const noexcept { return m_str; }
		inline const ostringstream_type& oss() const noexcept { return m_oss; }
		inline const _Elem* c_str() const noexcept { return m_str.c_str(); }

		template <class T>
		inline StringBuffer& operator<<(const T& data)
		{
			m_oss << data;
			return *this;
		}

		inline void Reserve(size_t newCapacity) { m_str.reserve(newCapacity); }
		inline void Clear() { m_str.clear(); }

	private:
		string_type m_str;
		ostringstream_type m_oss;
	};

	std::wstring GenerateNickname();
}
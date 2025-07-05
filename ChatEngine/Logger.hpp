#pragma once

#include "pch.hpp"

namespace debug
{
	enum class LogType { Print, Info, Warning, Error, Debug };

	template <class _Elem>
	class BasicLoggerStream
	{
		LogType type;
		std::basic_ostringstream<_Elem, std::char_traits<_Elem>, std::allocator<_Elem>> oss;

	public:
		BasicLoggerStream(LogType type);
		~BasicLoggerStream();

		BasicLoggerStream(const BasicLoggerStream&) = default;
		BasicLoggerStream(BasicLoggerStream&&) = default;
		BasicLoggerStream& operator=(const BasicLoggerStream&) = default;
		BasicLoggerStream& operator=(BasicLoggerStream&&) = default;

		inline LogType getType() const noexcept { return type; }

		template <class T>
		inline BasicLoggerStream& operator<<(const T& data)
		{
			oss << data;
			return *this;
		}
	};

	using LoggerStream = BasicLoggerStream<char>;
	using WLoggerStream = BasicLoggerStream<wchar_t>;

	void log(LogType type, const std::string& message);
	void log(LogType type, const std::wstring& message);

	inline void wlog(LogType type, const std::wstring& message) noexcept { log(type, message); }

	inline void print    (const std::string& message) noexcept { log(LogType::Print, message);   }
	inline void info     (const std::string& message) noexcept { log(LogType::Info, message);    }
	inline void warning  (const std::string& message) noexcept { log(LogType::Warning, message); }
	inline void error    (const std::string& message) noexcept { log(LogType::Error, message);   }
	inline void debug    (const std::string& message) noexcept { log(LogType::Debug, message);   }

	inline void print   (const std::wstring& message) noexcept { log(LogType::Print, message);   }
	inline void info    (const std::wstring& message) noexcept { log(LogType::Info, message);    }
	inline void warning (const std::wstring& message) noexcept { log(LogType::Warning, message); }
	inline void error   (const std::wstring& message) noexcept { log(LogType::Error, message);   }
	inline void debug   (const std::wstring& message) noexcept { log(LogType::Debug, message);   }

	inline void wprint   (const std::wstring& message) noexcept { log(LogType::Print, message);   }
	inline void winfo    (const std::wstring& message) noexcept { log(LogType::Info, message);    }
	inline void wwarning (const std::wstring& message) noexcept { log(LogType::Warning, message); }
	inline void werror   (const std::wstring& message) noexcept { log(LogType::Error, message);   }
	inline void wdebug   (const std::wstring& message) noexcept { log(LogType::Debug, message);   }

	inline LoggerStream  print()    noexcept { return LoggerStream (LogType::Print);   }
	inline LoggerStream  info()     noexcept { return LoggerStream (LogType::Info);    }
	inline LoggerStream  warning()  noexcept { return LoggerStream (LogType::Warning); }
	inline LoggerStream  error()    noexcept { return LoggerStream (LogType::Error);   }
	inline LoggerStream  debug()    noexcept { return LoggerStream (LogType::Debug);   }

	inline WLoggerStream wprint()   noexcept { return WLoggerStream(LogType::Print);   }
	inline WLoggerStream winfo()    noexcept { return WLoggerStream(LogType::Info);    }
	inline WLoggerStream wwarning() noexcept { return WLoggerStream(LogType::Warning); }
	inline WLoggerStream werror()   noexcept { return WLoggerStream(LogType::Error);   }
	inline WLoggerStream wdebug()   noexcept { return WLoggerStream(LogType::Debug);   }

	// Templates

	template<class _Elem>
	inline BasicLoggerStream<_Elem>::BasicLoggerStream(LogType type) : type{type} { }

	template<class _Elem>
	inline BasicLoggerStream<_Elem>::~BasicLoggerStream() { log(type, oss.str()); }
}
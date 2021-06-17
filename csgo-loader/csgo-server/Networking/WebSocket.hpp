#pragma once

#include <windows.h>
#include <wininet.h>
#include <vector>
#include <algorithm>
#include <cstdint>

#pragma comment(lib, "wininet.lib")

using ByteArray = std::vector<uint8_t>;

namespace Networking
{
	// Whenever the handle goes out of scope, it will automatically be released.
	class InternetHandle
	{
		HINTERNET m_Internet;
	public:
		InternetHandle() = default;
		InternetHandle(HINTERNET Internet) :
			m_Internet(Internet) { }

		~InternetHandle()
		{
			InternetCloseHandle(m_Internet);
		}

		operator HINTERNET() { return m_Internet; };
	};

	class WebSocket
	{
		InternetHandle m_Internet;
		InternetHandle m_Address;

	public:
		bool Start(const char *Address, const char *Username, const char *Password);
		ByteArray Request(const char *File, const char *Header, ByteArray &Data);
	};
}
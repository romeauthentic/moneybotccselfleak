#pragma once

#include <fstream>
#include <cstdint>
#include <vector>
#include <iterator>

using ByteArray = std::vector< uint8_t >;

namespace RemoteCode
{
	class FileReader
	{
		// Contents of the file.
		ByteArray m_Contents;

	public:
		FileReader() = default;

		// Constructor (ignores exception-handling).
		FileReader(const char *FileName) { Start(FileName); }

		// Read a file.
		bool Start(const char *FileName);

		// Self-explanatory.
		size_t GetFileLength() { return m_Contents.size(); }

		// Allow the user to walk the data.
		operator uint8_t *() { return m_Contents.data(); }
	};
}
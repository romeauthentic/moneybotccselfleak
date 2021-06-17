#include <RemoteCode/FileReader.hpp>

namespace RemoteCode
{
	bool FileReader::Start(const char *FileName)
	{
		std::ifstream File(FileName, std::ios::in | std::ios::binary);

		// File does not exist/is not open.
		if(!File.is_open())
			return false;

		// Do not skip white-space, read file.
		File.unsetf(std::ios::skipws);
		m_Contents.insert(
			m_Contents.begin(),
			std::istream_iterator<uint8_t>(File),
			std::istream_iterator<uint8_t>()
		);

		if(m_Contents.empty())
			return false;

		// Close the handle.
		File.close();

		return true;
	}
}
#include <Networking/WebSocket.hpp>

namespace Networking
{
	// Initialises a basic HTTP socket.
	bool WebSocket::Start(const char *Address, const char *Username, const char *Password)
	{
		m_Internet = InternetOpenA("none", INTERNET_OPEN_TYPE_PRECONFIG, 0, 0, 0);

		if(!m_Internet)
			return false;

		m_Address = InternetConnectA(m_Internet, Address, INTERNET_DEFAULT_HTTPS_PORT, Username, Password, INTERNET_SERVICE_HTTP, 0, 0);

		if(!m_Address)
			return false;

		return true;
	}

	// Receives a response from a request.
	ByteArray WebSocket::Request(const char *File, const char *Header, ByteArray &Data)
	{
		ByteArray	   Response;
		InternetHandle WebRequest = HttpOpenRequestA(m_Address, "POST", File, 0, 0, 0, INTERNET_FLAG_SECURE | INTERNET_FLAG_KEEP_CONNECTION, 0);

		// Make connection request.
		bool Sent = HttpSendRequestA(WebRequest, Header, (DWORD)strlen(Header), Data.data(), (DWORD)Data.size());

		if(Sent)
		{
			DWORD ReceivedSize{};

			uint8_t *Block = (uint8_t *)malloc(4096);

			// Read response.
			while(InternetReadFile(WebRequest, Block, 4096, &ReceivedSize))
			{
				for(size_t n{}; n < std::min< int >(4096, ReceivedSize); ++n)
				{
					Response.push_back(Block[n]);
				}
			}

			free(Block);
		}

		return Response;
	}
}
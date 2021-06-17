#include <Networking/TCPClient.hpp>
#include <UserExperience/UserInterface.hpp>

namespace Networking
{
	// We will only receive up to 256 bytes per cycle.
	constexpr int BufferSize = 256;

	void TCPClient::SendRawBytes(ByteArray &Bytes)
	{
		// Send data.
		int32_t Result = send(m_Socket, (char *)Bytes.data(), (int)Bytes.size(), 0);

		if(Result == -1)
			INFO_ASSERT(STR("[000F:00002B00] Server closed the connection unexpectedly."));

		WRAP_IF_DEBUG(
			size_t BreakIndex = 0;

			printf("[DEBUG] Sending data (%zd bytes):\n[DEBUG] ", Bytes.size());

			for(auto &Byte : Bytes)
			{
				printf("%02x ", Byte);

				if(BreakIndex++ % 8 == 0)
					printf("\n[DEBUG] ");
			}

			printf("\n");
		);
	}

	ByteArray TCPClient::ReceiveRawBytes()
	{
		ByteArray	ReceivedBytes;
		uint8_t		RecvBuffer[BufferSize];

		// Attempt to receive a packet.
		while(true)
		{
			int32_t Received = recv(m_Socket, (char*)RecvBuffer, BufferSize, 0);

			// No more bytes left to receive.
			if(Received < 0)
				break;

			// Emplace all received bytes.
			for(int n = 0; n < Received; ++n)
			{
				ReceivedBytes.push_back(RecvBuffer[n]);
			}

			// No more bytes left to receive.
			if(Received < BufferSize)
				break;
		}

		WRAP_IF_DEBUG(
			size_t BreakIndex = 0;

			printf("[DEBUG] Receiving data (%zd bytes):\n[DEBUG] ", ReceivedBytes.size());

			for(auto &Byte : ReceivedBytes)
			{
				printf("%02x ", Byte);

				if(BreakIndex++ % 8 == 0)
					printf("\n[DEBUG] ");
			}

			printf("\n");
		);

		return ReceivedBytes;
	}

	void TCPClient::SendBytes(ByteArray &Bytes)
	{
		// Encrypt outgoing data.
		ByteArray EncryptionKey;
		EncryptionKey.insert(
			EncryptionKey.begin(),
			m_EncryptionKey,
			m_EncryptionKey + sizeof m_EncryptionKey
		);

		Wrapper::Encryption Encryption; Encryption.Start(EncryptionKey);

		ByteArray Encrypted = Encryption.Encrypt(Bytes);

		SendRawBytes(Encrypted);
	}

	ByteArray TCPClient::ReceiveBytes()
	{
		// Decrypt incoming data.
		ByteArray ReceivedBytes = ReceiveRawBytes();

		ByteArray EncryptionKey;
		EncryptionKey.insert(
			EncryptionKey.begin(),
			m_EncryptionKey,
			m_EncryptionKey + sizeof m_EncryptionKey
		);

		Wrapper::Encryption Encryption; Encryption.Start(EncryptionKey);

		ByteArray Decrypted = Encryption.Decrypt(ReceivedBytes);

		return Decrypted;
	}

	bool TCPClient::Start(uint32_t ServerAddress, uint16_t ServerPort)
	{
		const int32_t version = 0x101;

		// Initialise WinSocks.
		if(WSAStartup(version, &m_WinSocks))
			return false;

		// Create an IPv4 socket.
		m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if(m_Socket == INVALID_SOCKET)
			return false;

		// Set up client context.
		m_Context.sin_addr.s_addr = ServerAddress;
		m_Context.sin_family      = AF_INET;
		m_Context.sin_port        = htons(ServerPort);

		// Attempt connection.
		if(connect(m_Socket, (sockaddr *)&m_Context, sizeof m_Context))
			return false;

		// Initialise encryption wrapper.
		WRAP_IF_DEBUG(
			printf("[DEBUG] Attempting to handshake with server...\n");
		);

		ByteArray EncryptionKey = ReceiveRawBytes();
		
		if(EncryptionKey.empty())
			return false;

		std::memcpy(m_EncryptionKey, EncryptionKey.data(), EncryptionKey.size());

		WRAP_IF_DEBUG(
			printf("[DEBUG] Server handshake successful!\n");
		)

		return true;
	}

	void TCPClient::Kill()
	{
		if(m_Socket)
			closesocket(m_Socket);

		WSACleanup();
	}
}
#include <Networking/TCPServer.hpp>

namespace Networking
{
	void TCPConnection::Close()
	{
		printf("[ <= ] %s disconnected!\n", m_IpAddress);

		if(m_Socket)
			closesocket(m_Socket);
	}

	// We will only receive up to 256 bytes per cycle.
	constexpr int BufferSize = 256;

	void TCPConnection::SendRawBytes(ByteArray &Bytes)
	{
		// Send data.
		int32_t Result = send(m_Socket, (char *)Bytes.data(), (int)Bytes.size(), 0);

		printf("[ => ] Sending %zd bytes to %s.\n", Bytes.size(), m_IpAddress);

		if(Result == -1)
			printf("[ E! ] Failed to send %zd bytes to %s. (Socket %04Ix)\n", Bytes.size(), m_IpAddress, m_Socket);
	}

	ByteArray TCPConnection::ReceiveRawBytes()
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

		return ReceivedBytes;
	}

	void TCPConnection::SendBytes(ByteArray &Bytes)
	{
		// Encrypt outgoing data.
		ByteArray Encrypted = m_Encryption.Encrypt(Bytes);

		SendRawBytes(Encrypted);
	}

	ByteArray TCPConnection::ReceiveBytes()
	{
		ByteArray ReceivedBytes = ReceiveRawBytes();

		// Decrypt incoming data.
		ByteArray Decrypted = m_Encryption.Decrypt(ReceivedBytes);

		return Decrypted;
	}

	bool TCPServer::Start(uint16_t ServerPort)
	{
		const int32_t version = 0x101;

		// Initialise WinSocks.
		if(WSAStartup(version, &m_WinSocks))
			return false;

		// Create an IPv4 socket.
		m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if(m_Socket == INVALID_SOCKET)
			return false;

		// Set up server context.
		m_Context.sin_addr.s_addr = INADDR_ANY;
		m_Context.sin_family      = AF_INET;
		m_Context.sin_port        = htons(ServerPort);

		int32_t Bind = bind(m_Socket, (sockaddr *)&m_Context, sizeof sockaddr_in);

		if(Bind == INVALID_SOCKET)
			return false;

		// Start listening.
		printf("[ i! ] Server listening on port %d.\n", ServerPort);
		listen(m_Socket, 1);

		return true;
	}

	void TCPServer::AcceptConnection()
	{
		sockaddr_in IncomingConnection;
		int32_t     AddressLength = sizeof IncomingConnection;

		// Accept the incoming connection.
		SOCKET IncomingSocket = accept(m_Socket, (sockaddr *)&IncomingConnection, &AddressLength);

		if(IncomingSocket != INVALID_SOCKET)
		{
			Wrapper::Encryption Encryption;

			// Initialise encryption context.
			Encryption.Start();

			// Attempt handshake with client.
			const char *IpAddress = inet_ntoa(IncomingConnection.sin_addr);
			TCPConnection Connection(IncomingSocket, IpAddress, Encryption);

			ByteArray EncryptionKey = Connection.GetEncryptionKey();
			Connection.SendRawBytes(EncryptionKey);

			// Detach a thread to handle the connection.
			std::thread thread([&]
			{
				// Fix for crashing when there is no connection handler
				// (literally happened to me once)
				if(m_ConnectionHandler)
					m_ConnectionHandler(Connection);

				Connection.Close();
			});
			thread.detach();
		}
	}
}
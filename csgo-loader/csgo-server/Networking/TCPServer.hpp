#pragma once

// For encryption wrappers.
#include <Security/Encryption.hpp>

// WinSocks
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

// std::function
#include <functional>

// std::min
#include <algorithm>

// std::thread
#include <thread>

namespace Networking
{
	// Base connection class, used to handle multiple connections in a thread-based model.
	class TCPConnection
	{
		SOCKET				m_Socket;
		Wrapper::Encryption m_Encryption;
		char				m_IpAddress[32];

	public:
		// Initialiser for TCPConnection class.
		TCPConnection(SOCKET Connection, const char *IpAddress, Wrapper::Encryption &RSA) :
			m_Encryption(RSA), m_Socket(Connection)
		{
			strcpy_s<32>(m_IpAddress, IpAddress);
			printf("[ => ] %s connected!\n", m_IpAddress);
		}

		// Release the connection once it goes out of scope.
		void Close();

		// Wrappers for sending/receiving data.
		void SendRawBytes(ByteArray &Bytes);
		ByteArray ReceiveRawBytes();

		void SendBytes(ByteArray &Bytes);
		ByteArray ReceiveBytes();

		// Overload for getting the socket, in case we need to expose it.
		SOCKET operator()()
		{
			return m_Socket;
		}

		// Expose the encryption key for the connection.
		ByteArray GetEncryptionKey()
		{
			return m_Encryption.GetKey();
		}

		char *GetIpAddress() { return m_IpAddress; }
	};

	// Basic TCP server. Supports custom connection handling (pass a lambda to the handler list).
	using ConnectionHandler = std::function<void(TCPConnection &)>;

	class TCPServer
	{
		WSADATA		m_WinSocks;
		SOCKET		m_Socket;
		sockaddr_in m_Context;

		// Connection handlers, will be called sequentially upon connection.
		ConnectionHandler m_ConnectionHandler;

	public:
		// Default constructor, nothing needed for now.
		TCPServer() = default;

		// Handle destruction of server once it goes out of scope.
		~TCPServer()
		{
			// If we have a socket, close it.
			if(m_Socket)
				closesocket(m_Socket);

			// Close WSA context.
			WSACleanup();
		}

		// Handle the creation and handling of TCPServer connections.
		bool Start(uint16_t ServerPort);
		void AcceptConnection();

		// Overload for adding connection handlers, C# style support for events.
		void operator+=(std::function<void(TCPConnection &)> Function)
		{
			m_ConnectionHandler = Function;
		}
	};
}
#pragma once

// For encryption wrappers.
#include <Security/Encryption.hpp>

// WinSocks
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

// std::min 
#include <algorithm>

namespace Networking
{
	// A TCPClient is essentially the same as the TCPConnection counterpart on the server,
	// however, it independently handles connection.
	class TCPClient
	{
		WSADATA					m_WinSocks;
		SOCKET					m_Socket;
		sockaddr_in				m_Context;
		uint8_t					m_EncryptionKey[32];

	public:
		TCPClient() = default;

		// Connects to a remote server.
		// Also handles the initial handshake between server and client.
		bool Start(uint32_t ServerAddress, uint16_t ServerPort);

		// Kills the client.
		void Kill();

		// Wrappers for sending/receiving data.
		void SendRawBytes(ByteArray &Bytes);
		ByteArray ReceiveRawBytes();

		void SendBytes(ByteArray &Bytes);
		ByteArray ReceiveBytes();
	};

	using TCPClientPtr = std::unique_ptr<TCPClient>;
}
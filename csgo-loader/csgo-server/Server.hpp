#pragma once

// NOTE:
// THE FOLLOWING MACROS ARE USED ONLY IN SERVER.CPP
// PLEASE UPDATE THEM ACCORDINGLY.
#define SERVER_PORT 0xF2C      // Hexadecimal representation of the server port.

#define CLIENT_HEADER 0x4D42310A
#define MODULE_HEADER 0x4D42320A
#define BANREQ_HEADER 0x4D42330A

// Core functionality
#include <Networking/TCPServer.hpp>
#include <Networking/WebSocket.hpp>

// Loader functionality
#include <Login/RemoteLogin.hpp>

#include <RemoteCode/FileReader.hpp>
//#include <RemoteCode/RemoteInjectionServer.hpp>

// Let's separate up the connection handlers :)
namespace Handler
{
	// Branches
	void OnClientConnection(Networking::TCPConnection &Connection);
	void OnModuleConnection(Networking::TCPConnection &Connection);
	void OnBanReqConnection(Networking::TCPConnection &Connection);

	// Default handler
	void OnReceiveConnection(Networking::TCPConnection &Connection);
}

// It looked nasty in Server.cpp, so I'm putting it here.
namespace Utils
{
	inline void OpenConsole()
	{
		// Create instance of console.
		AllocConsole();

		// Allow console to access output stream.
		FILE *file;
		freopen_s(&file, "CONOUT$", "w", stdout);

		// :^)
		SetConsoleTitleA("moneyserver $");
	}
}
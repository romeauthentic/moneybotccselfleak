#pragma once

// NOTE:
// THE FOLLOWING MACROS ARE USED ONLY IN CLIENT.CPP
// PLEASE UPDATE THEM ACCORDINGLY.
#define LOCAL_IP    0x0100007F // '127.0.0.1'
#define SERVER_IP   0xE53CA523 // Hexadecimal representation of the server IP, obtained by inet_addr()
#define SERVER_PORT 0xF2C      // Hexadecimal representation of the server port.

#define CLIENT_HEADER 0x4D42310A

// Security features (these will be initialised and ran 
// first, failure will terminate loader execution).
#include <Security/RuntimeSecurity.hpp>
#include <Security/SyscallManager.hpp>

// Core functionality
#include <Networking/TCPClient.hpp>
#include <UserExperience/UserInterface.hpp>

// Loader functionality
#include <Login/RemoteLogin.hpp>

#include <RemoteCode/RemoteProcess.hpp>
#include <RemoteCode/RemoteMapper.hpp>

// It looked nasty in Client.cpp, so I'm putting it here.
namespace Utils
{
	inline void OpenConsole()
	{
		// Create instance of console.
		AllocConsole();

		// Allow console to access output stream.
		FILE *file;
		freopen_s(&file, STR("CONOUT$"), STR("w"), stdout);

		// :^)
		SetConsoleTitleA(STR("moneyclient $"));
	
		printf(STR("[DEBUG] Ready\n"));
	}
}
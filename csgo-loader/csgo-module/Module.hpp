#pragma once

// NOTE:
// THE FOLLOWING MACROS ARE USED ONLY IN CLIENT.CPP
// PLEASE UPDATE THEM ACCORDINGLY.
#define LOCAL_IP    0x0100007F // '127.0.0.1'
#define SERVER_IP   0xE53CA523 // Hexadecimal representation of the server IP, obtained by inet_addr()
#define SERVER_PORT 0xF2C      // Hexadecimal representation of the server port.


// Security features (these will be initialised and ran 
// first, failure will terminate loader execution).
#include <Security/SyscallManager.hpp>

// Core functionality
#include <Networking/TCPClient.hpp>

// Required for the SDK from VMP which offers
// virtual machines and string encryption, as 
// well as debug/VM checks.
#include <VMProtectSDK.h>

// Just a neat little feature that I decided to implement :-)
#ifdef DEBUG
	// Sick macros, retard.
#define WRAP_IF_RELEASE( s )
#define WRAP_IF_DEBUG( s ) { s; }

#define STR( s ) s
#else
	// Sick macros, retard.
#define WRAP_IF_RELEASE( s ) { s; }
#define WRAP_IF_DEBUG( s )

#define STR( s ) VMProtectDecryptStringA( s )
#endif

// It looked nasty in Module.cpp, so I'm putting it here.
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
		SetConsoleTitleA(STR("moneymodule $"));

		printf(STR("[DEBUG] Ready\n"));
	}
}
#pragma once

#include <cstdint>
#include <algorithm>
#include <vector>

using ByteArray = std::vector<uint8_t>;

namespace Login
{
	// Login header that is sent over to the server
	struct RemoteLoginHeader
	{
		// The first four bytes are encoded by the client.
		// This will carry the client version which can be checked.
		uint32_t    m_ClientHeader;

		// The username is raw text.
		// TODO: Hash the password client-side.
		char		m_Username[128];
		char		m_Password[128];

		// This will provide the hardware ID of the machine.
		uint64_t	m_HardwareId;

		// These fields will be set according
		// to security check results.
		uint8_t		m_IntegrityBit1; // Detour detected on NTDLL function
		uint8_t		m_IntegrityBit2; // Detour detected on dummy function
		uint8_t     m_IntegrityBit3; // Virtual machine/Debugger detected
		uint8_t		m_IntegrityBit4; // m_IntegrityBit1 | m_IntegrityBit2 | m_IntegrityBit3 (checksum)
	};

	// Possible server responses
	// The hardware ID is encoded (XORed with the message ID) within the message for
	// shadow ban/forum ban purposes. :)
	enum RemoteLoginResponse : uint8_t
	{
		OUTDATED_CLIENT		= 'A', // '[000A:{HWID}] Your client is outdated. Please download the latest client at 'moneybot.cc'.'
		ACCESS_AUTHORISED	= 'B', // Allows the user to continue with injection.
		INVALID_CREDENTIALS = 'C', // '[000C:{HWID}] Your credentials are invalid. Please check your spelling and try again.'
		USER_BANNED			= 'D', // '[000D:{HWID}] Your account is banned. Please contact 'admin@moneybot.cc' for additional information.'
		INVALID_HARDWARE	= 'E', // '[000E:{HWID}] Please contact an administrator to request a hardware ID reset.'
		INTEGRITY_FAILURE	= 'F', // '[000F:{HWID}] Failed to verify session. Please contact an administrator.' AKA the 'shadow ban', blacklists user from loader but not from forums.
		NO_SUBSCRIPTION		= 'G', // '[000G:{HWID}] No active subscription.'
		ACCESS_SPECIAL_USER = 'H', // Allows the user to continue, sets the m_SpecialAccess var
	};

	// Implementation of the server (handles login bullshit).
	class RemoteLoginServer
	{
		RemoteLoginHeader m_Header;

		// Polls the server for data, responds with whether or not the client
		// is allowed to use the cheat.
		RemoteLoginResponse GetLoginResponse();

	public:
		// Initialises the login header.
		bool Start(ByteArray &RawLoginHeader);

		ByteArray GetResponse();

		// Expose the header for use with other classes.
		RemoteLoginHeader GetHeader() { return m_Header; }

		// TODO: Implement shadow banning based on IP and HWID.
	};
}
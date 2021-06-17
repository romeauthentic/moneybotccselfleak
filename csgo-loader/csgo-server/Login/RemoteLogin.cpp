#include <Login/RemoteLogin.hpp>

// Change this whenever a major server update is made.
// NOTE: You must change this on the client as well.
#define EXPECTED_CLIENT_HEADER 0x62746324

namespace Login
{
	bool RemoteLoginServer::Start(ByteArray &RawLoginHeader)
	{
		if(RawLoginHeader.empty())
			return false;

		// Epic direct casts :---DDDD
		m_Header = *(RemoteLoginHeader *)&RawLoginHeader[0];

		printf("[ => ] User attempting login: %s\n", m_Header.m_Username);
		printf("[ => ] User HWID: %llx\n", m_Header.m_HardwareId);
		
		RemoteLoginResponse Response = GetLoginResponse();

		printf("[ => ] User response: %02x\n", Response);

		if(Response & ACCESS_AUTHORISED || Response & ACCESS_SPECIAL_USER)
			return true;
		
		printf("[ => ] Login for user %s was rejected!\n", m_Header.m_Username);

		return false;
	}

	RemoteLoginResponse RemoteLoginServer::GetLoginResponse()
	{
		// The header seems to be wrong, tell the client to update.
		if(m_Header.m_ClientHeader != EXPECTED_CLIENT_HEADER)
			return RemoteLoginResponse::OUTDATED_CLIENT;

		// TODO: Check if the user is banned.
		if(false)
			return RemoteLoginResponse::USER_BANNED;

		// TODO: Login the user.
		if(strcmp(m_Header.m_Username, "betauser"))
			return RemoteLoginResponse::INVALID_CREDENTIALS;

		if(strcmp(m_Header.m_Password, "betapassword"))
			return RemoteLoginResponse::INVALID_CREDENTIALS;

		if(!m_Header.m_HardwareId)
		{
			// TODO: Shadow ban the user.

			return RemoteLoginResponse::INTEGRITY_FAILURE;
		}

		// TODO: Check if the HWID is present in DB.
		if(m_Header.m_HardwareId != 0xd33a13f59ae35130)
			return RemoteLoginResponse::INVALID_HARDWARE;

		// TODO: Check if the user has a subscription.
		if(false)
			return RemoteLoginResponse::NO_SUBSCRIPTION;


		// Checksum validation.
		uint8_t Checksum = m_Header.m_IntegrityBit1
					     | m_Header.m_IntegrityBit2
					     | m_Header.m_IntegrityBit3;

		if(Checksum || Checksum != m_Header.m_IntegrityBit4)
		{
			// TODO: Shadow ban the user.

			return RemoteLoginResponse::INTEGRITY_FAILURE;
		}

		return RemoteLoginResponse::ACCESS_AUTHORISED;
	}

	ByteArray RemoteLoginServer::GetResponse()
	{
		// The way the server handles data transmission is homosexual.
		// That is the only reason this autism is here.
		ByteArray Response;
		Response.push_back(GetLoginResponse());

		return Response;
	}
}
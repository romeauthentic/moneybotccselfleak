#include <Login/RemoteLogin.hpp>

// Change this whenever a major client update is made.
// NOTE: You must change this on the server as well.
#define CURRENT_CLIENT_HEADER 0x62746324

namespace Login
{
	void RemoteLoginTransaction::Start(const char *Username, const char *Password)
	{
		VMProtectBeginUltra("LoginTransactionStart");

		// Initialise the header with the client header.
		m_Header.m_ClientHeader = CURRENT_CLIENT_HEADER;

		// Initialise the header with the username and password.
		strcpy_s< 128 >(m_Header.m_Username, Username);
		strcpy_s< 128 >(m_Header.m_Password, Password);

		// Initialise the header with the Hardware ID.
		Security::HardwareIdentifier HardwareId = Protection->GetHardwareId();

		WRAP_IF_DEBUG(
			printf("[DEBUG] RemoteLoginTransaction Start\n");
			printf("[DEBUG] Processor count: %d\n", HardwareId.m_CpuCount);
			printf("[DEBUG] Processor architecture: %d\n", HardwareId.m_CpuArchitecture);
			printf("[DEBUG] Hard-drive Serial: %llx\n", HardwareId.m_HardDiskSerialHash);
			printf("[DEBUG] Detail: %s\n", HardwareId.m_CustomDetail);

			for(int i = 0; i < 4; ++i)
				printf("[DEBUG] Safety check #%d: %s\n", i, HardwareId.m_SpecialMode[i] ? "TRUE" : "FALSE");
		);

		m_Header.m_HardwareId = fnv::hash_runtime_data((void *)(&HardwareId), sizeof Security::HardwareIdentifier);

		WRAP_IF_DEBUG(
			printf("[DEBUG] Hardware-ID Hash: %llx\n", m_Header.m_HardwareId);
		);

		// TODO: Verify integrity of system.
		// 0 for integrity passed, random bit for failure
		m_Header.m_IntegrityBit1 = HardwareId.m_SpecialMode[Security::DEBUGGING_MODE]; 
		m_Header.m_IntegrityBit2 = HardwareId.m_SpecialMode[Security::TEST_BUILD_MODE];
		m_Header.m_IntegrityBit3 = HardwareId.m_SpecialMode[Security::TEST_SIGN_MODE];

		// The checksum bit, the server will check this first to detect possible tampering.
		m_Header.m_IntegrityBit4 = m_Header.m_IntegrityBit1
								 | m_Header.m_IntegrityBit2
								 | m_Header.m_IntegrityBit3;
	
		VMProtectEnd();
	}

	// TODO: Hardware ID check.

	bool RemoteLoginTransaction::TranslateResponse(ByteArray &RawResponse)
	{
		if(RawResponse.empty())
			return false;

		RemoteLoginResponse ServerResponse = *(RemoteLoginResponse *)&RawResponse[0];

		WRAP_IF_DEBUG(
			printf("[DEBUG] RemoteLoginTransaction Finish: %02x\n", ServerResponse);
		);

		switch(ServerResponse)
		{
			case RemoteLoginResponse::ACCESS_SPECIAL_USER:
				// Allow the user to load special access cheats.
				UserInterface->m_Data.m_SpecialAccess = true;
			case RemoteLoginResponse::ACCESS_AUTHORISED:
				return true;
			case RemoteLoginResponse::OUTDATED_CLIENT:
				ERROR_ASSERT(STR("[000A:%llx] Your client is outdated.\nPlease download the latest client at 'moneybot.cc'."), m_Header.m_HardwareId ^ ServerResponse);
				break;
			case RemoteLoginResponse::USER_BANNED:
				ERROR_ASSERT(STR("[000D:%llx] Your account is banned.\nPlease contact 'admin@moneybot.cc' for additional information."), m_Header.m_HardwareId ^ ServerResponse);
				break;
			case RemoteLoginResponse::INVALID_HARDWARE:
				ERROR_ASSERT(STR("[000D:%llx] Your Hardware-ID is incorrect!\nPlease contact a staff member."), m_Header.m_HardwareId ^ ServerResponse);
				break;
			case RemoteLoginResponse::INVALID_CREDENTIALS:
				ERROR_ASSERT(STR("[000C:%llx] Your credentials are invalid. Please check your spelling and try again."), m_Header.m_HardwareId ^ ServerResponse);
				break;
			case RemoteLoginResponse::INTEGRITY_FAILURE:
			case RemoteLoginResponse::NO_SUBSCRIPTION:
				INFO_ASSERT(STR("[0005:%llx] No active subscription found."), m_Header.m_HardwareId ^ ServerResponse);
				break;
		}

		return false;
	}
}
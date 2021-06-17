#include <Server.hpp>

// 'M1' -> cl request
// 'M2' -> mod request
// 'M3' -> ban request

namespace Handler
{
	void OnClientConnection(Networking::TCPConnection &Connection)
	{
		printf("[ !! ] Client at %s requested connection!\n", Connection.GetIpAddress());

		// Initialize login server for the client.
		Login::RemoteLoginServer LoginServer;

		ByteArray LoginHeader = Connection.ReceiveBytes();

		// Invalid login header.
		if(!LoginServer.Start(LoginHeader))
			return;

		printf("[ !! ] Received login header from %s!\n", Connection.GetIpAddress());

		// Reply with server header.
		ByteArray LoginReply = LoginServer.GetResponse();
		Connection.SendBytes(LoginReply);

		ByteArray LoginReplyEcho = Connection.ReceiveBytes();

		if(LoginReply.size() != LoginReplyEcho.size())
		{
			printf("[ !! ] Echo from %s invalid, dropping connection!\n", Connection.GetIpAddress());
			return;
		}

		RemoteCode::FileReader File;

		if(!File.Start("csgo-module.dll"))
			return;

		// Send them the loader module to inject the cheat.
		printf("[ !! ] Sending latest loader module!\n");

		ByteArray RawLdrModule;
		RawLdrModule.insert(
			RawLdrModule.begin(),
			(uint8_t *)File,
			(uint8_t *)(File + File.GetFileLength())
		);

		Connection.SendBytes(RawLdrModule);
	}

	void OnModuleConnection(Networking::TCPConnection &Connection)
	{
		// The output of this function will be verbose by default.
		printf("[ !! ] Module hello from %s!\n", Connection.GetIpAddress());
	}

	void OnBanReqConnection(Networking::TCPConnection &Connection)
	{
		// Use for forum IP-ban purposes or whatever..
		printf("[ !! ] Client at %s requested ban!\n", Connection.GetIpAddress());

		// TODO: Ban user?
	}

	void OnReceiveConnection(Networking::TCPConnection &Connection)
	{
		ByteArray Header = Connection.ReceiveRawBytes();

		if(Header.empty())
		{
			printf("[ !! ] Client at %s sent malformed request!\n", Connection.GetIpAddress());
			return;
		}

		uint32_t HeaderCode = *(uint32_t *)&Header[0];

		switch(HeaderCode)
		{
			case CLIENT_HEADER: // "MB1"
				OnClientConnection(Connection); break;
			case MODULE_HEADER: // "MB2"
				OnModuleConnection(Connection); break;
			case BANREQ_HEADER: // "MB3";
				OnBanReqConnection(Connection); break;

			// Drop any malformed clients.
			default:
				printf("[ !! ] Client at %s sent malformed request!\n", Connection.GetIpAddress());
		}
	}
}

int __stdcall WinMain(HINSTANCE, HINSTANCE, char *, int)
{
	// Open a debugging console.
	Utils::OpenConsole();
	
	// Create an instance of the TCP server.
	Networking::TCPServer Server;

	bool Result = Server.Start(SERVER_PORT);

	if(Result)
	{
		// Attach our connection handler.
		Server += Handler::OnReceiveConnection;

		// Accept any incoming connections.
		for(;;)
			Server.AcceptConnection();
	}

	if(!Result)
		printf("[ E! ] Failed to initialise server. (%08lx)\n", WSAGetLastError());

	system("pause");
}
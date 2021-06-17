#include <Client.hpp>

/*
	--nave note:
		you don't need any custom params
		it's entirely pointless, we can use a wiped PE header to pass all the data we need
		plus passing shit via dllmain is too obvious anyway, dumb idea

	--nave note 2:
		what you haven't done and what will completely fuck us up if anyone competent gets the loader:
		loading the binary directly via the loader itself is completely stupid and unreasonable
		this is exactly what got rifk7 detected back in the day because all valve has to do is just get the loader and sig it
	
		what you're supposed to do is have a dummy dll which gets downloaded after login/security checks, inject that into a random process (explorer.exe will do) 
		that later downloads the REAL dll from the server and then injects it
		otherwise all it takes is 1 memer to send it to valve and we'll get fucked completely

	TODO (Nave):
		- Make the UI look nice.
		- Adapt the server to work with your backend.
		- Add dump protection (closes csgo.exe if a handle is detected, probably explorer shellcode)

	--nave note 3:
		dumps will happen anyway, i honestly don't even wanna focus on trying to protect shit against that other than basic exitprocess memes
		people dump skeet regardless of kernel address mapping meme so what does it matter
*/

int __stdcall WinMain(HINSTANCE, HINSTANCE, char *, int)
{
	WRAP_IF_DEBUG(
		Utils::OpenConsole();
	);

	//////////////////////////////////////////////////////////////////////////////////////////

	// Create a thread to handle UI.
	std::thread WindowThread([]
	{	
		// Create a window, initialise DirectX context.
		if(!UserInterface->Start())
			ERROR_ASSERT(STR("[000F:00001A00] Failed to initialize. Please contact an administrator."));
		
		UserInterface->RunUiFrame();
	}); WindowThread.detach();

	while(!UserInterface->m_Data.m_Ready) { Sleep(1); }

	//////////////////////////////////////////////////////////////////////////////////////////

	// Initialize the syscall manager.
	if(!Syscalls->Start())
		ERROR_ASSERT(STR("[000F:00001B00] Failed to initialize. Please contact an administrator."));

	// Initialize the runtime protection system.
	if(!Protection->Start())
		ERROR_ASSERT(STR("[000F:00001C00] Failed to initialize. Please contact an administrator."));

	// Wait for connection.
	UserInterface->m_Data.m_ExecutionState = UserExperience::EXECUTION_WAITING;

	// Attempt to connect to the remote server.
	WRAP_IF_DEBUG(
		printf("[DEBUG] Server IP: %08x\n", inet_addr("35.165.60.229"));
	);
	
	//////////////////////////////////////////////////////////////////////////////////////////

	// Connect to server.
	Networking::TCPClientPtr Client = std::make_unique<Networking::TCPClient>();

	if(!Client->Start(LOCAL_IP, SERVER_PORT))
		ERROR_ASSERT(STR("[000F:0002A000] Server closed the connection unexpectedly."));
	
	// Header for Client.
	ByteArray Header{ 0x0A, 0x31, 0x42, 0x4D };
	Client->SendRawBytes(Header);

	//////////////////////////////////////////////////////////////////////////////////////////

	// Allow the user to input their log-in data.
	UserInterface->m_Data.m_ExecutionState = UserExperience::EXECUTION_LOG_IN;

	while(UserInterface->m_Data.m_ExecutionState != UserExperience::EXECUTION_WAITING) { Sleep(1); }

	Login::RemoteLoginTransaction LoginTransaction;
	LoginTransaction.Start(UserInterface->m_Data.m_Username, UserInterface->m_Data.m_Password);

	ByteArray Transaction = LoginTransaction.GetHeader();
	Client->SendBytes(Transaction);

	ByteArray LoginResponse = Client->ReceiveBytes();
	if(!LoginTransaction.TranslateResponse(LoginResponse))
		return 1;

	// Echo back to server.
	Client->SendBytes(LoginResponse);

	//////////////////////////////////////////////////////////////////////////////////////////

	// Receive loader module.
	ByteArray LdrModule = Client->ReceiveBytes();

	// Map loader module.
	RemoteCode::RemoteProcess Process;

	if(!Process.Start(STR("explorer.exe")))
		return 1;

	RemoteCode::RemoteMapper Mapper;

	if(!Mapper.Start(Process, LdrModule))
		return 1;

	if(!Mapper.WriteCodeToMap())
		return 1;
	
	Mapper.ExecuteCodeFromMap();

	system("pause");

	return 0;
}
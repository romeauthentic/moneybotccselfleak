#include <Module.hpp>

/*
	TODO:
	- Finish off shellcode execution wrapper:
		- The shellcode can be executed via two ways
		- Either the code is mapped and called via CreateRemoteThread (allows custom param)
		- or the code is mapped and called via DX9 (does not allow custom param)
		- This will probably be the easiest thing to do.

	- Allocate via consecutive 64kb sections (TODO: Figure out how)

	- Free and wipe the module from memory once done
	- Have the module authenticate via HWID or some shit.. idk (AAAAAAAAAAA)
	- Do reloc/mapping stuff here
*/

DWORD ModuleThread(void *)
{
	//////////////////////////////////////////////////////////////////////////////////////////

	// Initialize the syscall manager.
	if(!Syscalls->Start())
		return 0;

	// Attempt to connect to the remote server.
	WRAP_IF_DEBUG(
		printf("[DEBUG] Server IP: %08x\n", inet_addr("35.165.60.229"));
	);

	//////////////////////////////////////////////////////////////////////////////////////////

	// Connect to server.
	Networking::TCPClientPtr Client = std::make_unique<Networking::TCPClient>();

	if(!Client->Start(LOCAL_IP, SERVER_PORT))
		return 0;

	// Header for Module.
	ByteArray Header{ 0x0A, 0x32, 0x42, 0x4D };
	Client->SendRawBytes(Header); 

	//////////////////////////////////////////////////////////////////////////////////////////

	return 1;
}

int __stdcall DllMain(void *, unsigned, void *)
{
	HANDLE Thread = CreateThread(nullptr, 0, ModuleThread, nullptr, 0, nullptr);

	if(Thread)
	{
		// Wait for thread to finish execution
		WaitForSingleObject(Thread, INFINITE);

		// Get exit code from thread.
		DWORD ExitCode;
		GetExitCodeThread(Thread, &ExitCode);

		// If the HWND is 0, the loader will reveal that MessageBoxA was called from
		// explorer.exe... This meme will get around that.
		HWND Window = FindWindowA(STR("Valve001"), nullptr);
		
		if(!Window)
			return true;

		if(!ExitCode)
			MessageBoxA(Window, STR("[000F:00004A00] Failed to initialize. Please contact an administrator."), "", MB_ICONERROR | MB_OK);
	}

	return true;
}
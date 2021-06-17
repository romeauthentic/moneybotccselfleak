#pragma once

#include <windows.h>
#include <thread>
#include <cstdint>
#include <cstdio>
#include <atomic>

namespace UserExperience
{
	// Execution states define a moment in the execution of the loader.
	// These may be changed externally by other threads.
	enum ExecutionState : uint16_t
	{
		EXECUTION_WAITING, // Displays the message 'please wait...'.
		EXECUTION_ERROR,   // Displays an error.
		EXECUTION_LOG_IN,  // Displays the log-in dialog.
		EXECUTION_CHOOSE   // Displays the game selection dialog.
	};

	enum ErrorReason : uint16_t
	{
		ERROR_GENERIC_ERROR,
		ERROR_INVALID_HWID,
		ERROR_SHADOW_BAN
	};

	enum SelectedGame : uint16_t
	{
		GAME_CSGO,
		GAME_CSGO_BETA,
		GAME_MAX
	};

	// Structure that holds global data that will be used by the UI.
	struct UserExperienceData
	{
		// Is the user interface initialised?
		bool           m_Ready          = false;
			
		// Holds the current execution state of the loader.
		ExecutionState m_ExecutionState = EXECUTION_WAITING;

		// Holds the username/password combo entered in the UI.
		char           m_Username[128];
		char           m_Password[128];

		// Does the user have special access?
		bool           m_SpecialAccess  = false;

		// Holds the selected game.
		uint16_t       m_SelectedGame   = GAME_CSGO;

		// Holds the current error message.
		ErrorReason    m_Error          = ERROR_GENERIC_ERROR;
	};

	// User experience handler.
	class UserInterface
	{
	public:
		UserExperienceData m_Data;

		// Creates a window.
		bool Start();

		// Creates an UI thread, call only once.
		void RunUiFrame();

		HWND GetWindow();
	};

	using UserInterfacePtr = std::unique_ptr<UserInterface>;
}

extern UserExperience::UserInterfacePtr UserInterface;

// Sick macros, retard.

#define ERROR_ASSERT(Error, ...) { char Buffer[1024 * 16]; sprintf_s(Buffer, sizeof Buffer, Error, __VA_ARGS__); MessageBoxA(UserInterface->GetWindow(), Buffer, "", MB_SYSTEMMODAL | MB_ICONERROR); ExitProcess(0); }
#define INFO_ASSERT(Error, ...)  { char Buffer[1024 * 16]; sprintf_s(Buffer, sizeof Buffer, Error, __VA_ARGS__); MessageBoxA(UserInterface->GetWindow(), Buffer, "", MB_SYSTEMMODAL | MB_OK); ExitProcess(0); }

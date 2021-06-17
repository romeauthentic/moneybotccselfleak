#pragma once

#include <RemoteCode/RemoteProcess.hpp>

namespace RemoteCode
{
	struct RemoteImport
	{
		char m_Module[128];
		char m_Import[128];
	};

	class RemoteMapper
	{
		RemoteProcess	m_Process;

		ByteArray		m_Code;
		ByteArray		m_Mapped;

		void		   *m_Map;

	public:
		RemoteMapper() = default;

		// Copy process & shellcode to class.
		bool Start(RemoteProcess &Process, ByteArray &Code);

		// Writes code to process in allocated page.
		bool WriteCodeToMap();

		// Calls shellcode to call the library ;D
		bool ExecuteCodeFromMap();
	};
}
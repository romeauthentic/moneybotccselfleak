#pragma once
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <xnamath.h>
#include <windows.h>

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "winmm.lib")

#include <vector>

#include "input_system.hpp"

namespace d3d
{
	typedef void( *on_frame_fn )( );

	class c_window {
		char m_window_name[32];
	public:
		c_window( );
		~c_window( );

		void generate_random_name();

		bool create( );
		void on_frame( );
		void add_on_frame( on_frame_fn fn ) {
			m_onframe_vec.push_back( fn );
		}

		HWND get_hwnd( ) { return m_hwnd; }

	private:
		bool init_d3d( );

	public:
		static LRESULT __stdcall window_procedure( HWND, UINT, WPARAM, LPARAM );

	private:
		HWND		m_hwnd{ };
		int			m_size[ 2 ]{ };

		WNDCLASSEX	m_wc{ };
		MSG			m_msg{ };

	public:
		LPDIRECT3DDEVICE9		m_d3d_device{ };
		D3DPRESENT_PARAMETERS	m_present_params{ };
		LPDIRECT3D9				m_d3d{ };

	private:
		std::vector< on_frame_fn > m_onframe_vec;
	};
}

extern d3d::c_window g_window;
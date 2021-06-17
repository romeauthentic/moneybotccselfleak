#pragma once
#include "window.hpp"
#include "d3d.hpp"
#include "math.hpp"
#include <time.h>

d3d::c_window g_window;

namespace d3d
{
	c_window::c_window() { m_size[0] = 451; m_size[1] = 376; generate_random_name(); } //ugh
	c_window::~c_window( ) { }

	void c_window::generate_random_name() {
		// math::random_number was NOT random
		// so fuck you and your use of STL
		srand((uint32_t)time(0));

		int i = 0;
		for(; i < ((rand()%31)+16); ++i) {
			m_window_name[i] = rand() & 0xFF;
		}

		m_window_name[++i] = 0;
	}

	LRESULT __stdcall c_window::window_procedure( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam ) {
		if( msg == WM_MOUSEMOVE ) {
			g_input.capture_mouse_move( lparam );
		}

		g_input.register_key_press( ( VirtualKeyEvents_t )( msg ), VirtualKeys_t( wparam ) );

		switch( msg ) {
		case WM_SIZE:
			if( g_window.m_d3d_device && wparam != SIZE_MINIMIZED ) {
				g_window.m_present_params.BackBufferWidth = LOWORD( lparam );
				g_window.m_present_params.BackBufferHeight = HIWORD( lparam );

				g_d3d.on_device_lost( );
				auto result = g_window.m_d3d_device->Reset( &g_window.m_present_params );
				g_d3d.on_device_reset( );
			}
			return 0;
		case WM_SYSCOMMAND:
			if( ( wparam & 0xfff0 ) == SC_KEYMENU ) //disable alt thing
				return 0;
			break;


		case WM_DESTROY:
			ExitProcess(0);
		}

		return DefWindowProc( hwnd, msg, wparam, lparam );
	}

	bool c_window::create( ) {
		m_wc.cbSize = sizeof( WNDCLASSEX );
		m_wc.style = CS_VREDRAW | CS_HREDRAW;
		m_wc.lpfnWndProc = window_procedure;
		m_wc.cbClsExtra = 0;
		m_wc.cbWndExtra = 0;
		m_wc.hInstance = 0;
		m_wc.hIcon = LoadIcon( 0, IDI_APPLICATION );
		m_wc.hCursor = LoadCursor( 0, IDC_ARROW );
		m_wc.lpszMenuName = 0;
		m_wc.lpszClassName = m_window_name;
		m_wc.hIconSm = LoadIcon( 0, IDI_APPLICATION );

		//m_wc.hbrBackground = ( HBRUSH )( RGB( 0, 0, 0 ) );

		RegisterClassEx( &m_wc );

		m_hwnd = CreateWindowExA( WS_EX_TRANSPARENT, m_window_name, m_window_name, WS_POPUP,
			CW_USEDEFAULT, CW_USEDEFAULT, m_size[ 0 ], m_size[ 1 ], nullptr, nullptr, nullptr, 0 );

		if( !m_hwnd ) {
			return false;
		}

		//SetLayeredWindowAttributes( m_hwnd, RGB( 0, 0, 0 ), 0, ULW_COLORKEY );
		//SetLayeredWindowAttributes( m_hwnd, RGB( 0, 0, 0 ), 255, LWA_ALPHA );
		ShowWindow( m_hwnd, SW_SHOWDEFAULT );
		UpdateWindow( m_hwnd );

		//MARGINS margin = { -1, -1, -1, -1 };
		//DwmExtendFrameIntoClientArea( m_hwnd, &margin );

		MoveWindow( m_hwnd, 20, 20, m_size[ 0 ], m_size[ 1 ], true );

		return init_d3d( );
	}

	bool c_window::init_d3d( ) {
		if( !( m_d3d = Direct3DCreate9( D3D_SDK_VERSION ) ) ) {
			UnregisterClass(m_window_name, m_wc.hInstance );
			return false;
		}

		ZeroMemory( &m_present_params, sizeof( m_present_params ) );

		m_present_params.Windowed = true;
		m_present_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_present_params.BackBufferFormat = D3DFMT_A8R8G8B8;
		m_present_params.hDeviceWindow = m_hwnd;
		m_present_params.EnableAutoDepthStencil = true;
		m_present_params.AutoDepthStencilFormat = D3DFMT_D16;
		m_present_params.MultiSampleType = D3DMULTISAMPLE_NONE;
		m_present_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		m_present_params.BackBufferCount = 1;
		m_present_params.BackBufferWidth = m_size[ 0 ];
		m_present_params.BackBufferHeight = m_size[ 1 ];

		if( m_d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_present_params, &m_d3d_device ) < 0 ) {
			m_d3d->Release( );
			UnregisterClass(m_window_name, m_wc.hInstance );
			return false;
		}

		return true;
	}

	void c_window::on_frame( ) {
		while( m_msg.message != WM_QUIT ) {
			if( PeekMessage( &m_msg, 0, 0, 0, PM_REMOVE ) ) {
				TranslateMessage( &m_msg );
				DispatchMessage( &m_msg );

				continue;
			}

			if( m_d3d_device ) {
				// Caused crashing when UAC prompt appeared.
				//auto device_state = m_d3d_device->TestCooperativeLevel( );
				//if( device_state != D3D_OK ) {
				//	g_d3d.on_device_lost( );
				//	m_d3d_device->Reset( &m_present_params );
				//	g_d3d.on_device_reset( );
				//}


				if( m_d3d_device->BeginScene( ) >= 0 ) {
					for( auto& onframe : m_onframe_vec ) {
						if( onframe ) {
							onframe( );
						}
					}
					//m_d3d_device->SetRenderState( D3DRS_ZENABLE, false );
					//m_d3d_device->SetRenderState( D3DRS_ALPHABLENDENABLE, false );
					//m_d3d_device->SetRenderState( D3DRS_SCISSORTESTENABLE, false );
					m_d3d_device->EndScene( );
				}

				m_d3d_device->Present( nullptr, nullptr, nullptr, nullptr );
				Sleep( 1 );
			}
		}
	}
}


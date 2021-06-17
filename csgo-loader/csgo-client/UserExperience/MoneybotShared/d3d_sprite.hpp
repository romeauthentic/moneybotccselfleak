#pragma once
#include <vector>
#include "d3d.hpp"


namespace d3d
{
	class c_sprite;

	extern std::vector< c_sprite* > sprites;

	class c_sprite {
	public:
		size_t m_width{ };
		size_t m_height{ };

		IDirect3DDevice9*  m_device{ };
		ID3DXSprite*	   m_sprite{ };
		IDirect3DTexture9* m_texture{ };
		const byte*		   m_image{ };
		size_t			   m_image_size{ };


	public:
		c_sprite( ) {
			sprites.push_back( this );
		}

		~c_sprite( ) {
			on_reset( );
		}

		void init( IDirect3DDevice9* device, const byte* file, size_t img_size, size_t width, size_t height ) {
			m_width = width;
			m_height = height;

			m_device = device;
			m_image = file;	
			m_image_size = img_size;
		}

		void begin( IDirect3DDevice9* device ) {
			m_device = device;

			if( !m_device ) {
				return;
			}

			if( !m_sprite )
				D3DXCreateSprite( m_device, &m_sprite );

			if( m_sprite )
				m_sprite->Begin( D3DXSPRITE_ALPHABLEND );

			if( !m_texture ) {
				auto hr = D3DXCreateTextureFromFileInMemoryEx(
					m_device, m_image, m_image_size,
					m_width, m_height, D3DX_DEFAULT, 0, D3DFMT_A8B8G8R8,
					D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0,
					0, 0, &m_texture );
			}
		}

		void end( ) {
			if( !m_device || !m_sprite || !m_texture ) return;
			if( m_sprite ) m_sprite->End( );
		}

		void on_reset( ) {
			if( m_sprite && m_device && m_texture ) {
				m_sprite->OnLostDevice( );
				m_sprite->OnResetDevice( );
				m_texture->Release( );
				m_texture = nullptr;
			}
		}

		void draw( int x, int y, clr_t color ) {
			if( !m_device || !m_texture || !m_sprite ) {
				return;
			}

			ulong_t hr;
			D3DXVECTOR2 center = D3DXVECTOR2( m_width * 0.5f, m_height * 0.5f );
			D3DXVECTOR2 trans = D3DXVECTOR2( x - center.x, y - center.y );
			D3DXMATRIX matrix;
			D3DXVECTOR2 scale( 1.f, 1.f );
			D3DXMatrixTransformation2D( &matrix, 0, 0.f, &scale, &center, 0.f, &trans );

			hr = m_sprite->SetTransform( &matrix );

			auto d3dcolor = D3DCOLOR_RGBA( color.r( ),
				color.g( ), color.b( ), color.a( ) );
			hr = m_sprite->Draw( m_texture, 0, 0, 0, d3dcolor );
		}
	};
}

namespace icons
{
	extern d3d::c_sprite sprite_legit;
	extern d3d::c_sprite sprite_visuals_;
	extern d3d::c_sprite sprite_rage;
	extern d3d::c_sprite sprite_visuals;
	extern d3d::c_sprite sprite_misc;
	extern d3d::c_sprite sprite_config;
}
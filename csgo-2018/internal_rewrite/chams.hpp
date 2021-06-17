#pragma once

#include "sdk.hpp"

namespace features
{
	enum ChamBufferType_t {
		TYPE_NORMAL,
		TYPE_FLAT,
		TYPE_SHINE
	};

	class c_material {
	public:
		c_material( ) = default;

		void init( const char* mat, const char* buf );
		void destroy( );

		operator IMaterial*( ) {
			return m_mat;
		}

	public:
		IMaterial* m_mat;
		KeyValues* m_keyvalues;
	};

	class c_materials {
	public:
		c_material m_chams{ };
		c_material m_chams_flat{ };

		void make_cham_buffer( char* buf, int len, int type ); //type, 0 = normal, 1 = flat, 2 = shine
	public:
		void initialize_materials( );
		void destroy_materials( );
		void force_material( IMaterial* material, fclr_t color );
		void update_materials( );
		bool m_initialized{ };
	};
	
	class c_chams {
	public:
		c_materials m_materials;
		void d3d_render_textures( );
		void d3d_render_chams( c_base_player* ent, int type, int v_index, uint32_t min_index, uint32_t num_vert, uint32_t start_index, uint32_t prim_count );
	};
}
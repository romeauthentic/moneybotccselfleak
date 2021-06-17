#pragma once

#include <cstring>

class c_base_weapon;
class IGameEvent;
namespace features
{
	class c_skins {
	private:
		int m_last_index{ 0 };
		bool m_needs_update{ };

		int get_knife_index( );
		const char* get_model_str( );
		
		const char* get_glove_model( );

		const char* get_killicon_str( );
		
		//void override_knife( );
		void override_gloves( );
		void override_weapon( c_base_weapon* wep );
		//void update_active_skin( );
		int get_current_weapon_id( );
		int get_weapon_id( c_base_weapon* );
		

	public:
		void operator()( );
		void replace_deaths( IGameEvent* event );
	};
}
#pragma once
#include <deque>
#include <chrono>

#include "sdk.hpp"

namespace features
{
	class c_legitbot {
	public:
		void operator()( float* x, float* y ) {
			aimbot( x, y );
		}

		//call once per tick
		void	sample_angle_data( const vec3_t& angle );
		void	triggerbot( user_cmd_t* cmd );
	private:
		struct aim_record_t {
			vec3_t m_viewangles;
			float m_time;
		};

		struct lag_record_t {
			int			m_tickcount{ };
			vec3_t		m_position{ };
			matrix3x4	m_matrix[ 128 ]{ };

			bool is_valid( );
		};

		class c_lagcomp {
		public:
			friend class c_legitbot;
			using lag_deque_t = std::deque< lag_record_t >;

			lag_record_t* find_best_record( int ent_index );
			vec3_t		  get_backtracked_position( int ent_index );
			auto&		  get_records( int ent_index ) {
				return m_data[ ent_index ];
			}

			void	store( );
			void	operator()( user_cmd_t* cmd );

		private:
			bool	backtrack_entity( int ent_index );
			bool	can_backtrack_entity( int ent_index );
			void	store_player( int ent_index );

			lag_deque_t m_data[ 65 ];
			user_cmd_t* m_cmd;
		};

		vec2_t	angle_to_pixels( const vec3_t& angle );
		vec3_t  pixels_to_angle( const vec2_t& pixel );
		void	aimbot( float* x, float* y );
		void	assist( c_base_player* player, float* x, float* y );
		void	aim_at_target( c_base_player*, float*, float* );

		bool	update_settings( );
		float	get_avg_delta( );
		int		get_aim_target( float fov = 0.f );

	private:
		float	m_deltatime{ };
		float	m_curtime{ };
		bool	m_aiming{ };

		std::deque< aim_record_t > m_angle_samples;

	public:
		c_lagcomp m_lagcomp;
	};
}
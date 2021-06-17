#pragma once
#include <deque>
#include <memory>

#include "sdk.hpp"
#include "console.hpp"

struct CCSGOPlayerAnimState;

enum RecordType_t {
	RECORD_NONE,
	RECORD_LBY,
	RECORD_NORMAL,
};

enum HitscanTypes_t {
	HITSCAN_OFF,
	HITSCAN_HEAD_BODY,
	HITSCAN_HEAD_BODY_ARMS,
	HITSCAN_LIMBS,
};

enum BreakerState_t {
	BS_OFF,
	BS_MOVING,
	BS_WAS_MOVING,
	BS_BREAKING,
};

struct lby_update_t {
	int m_tickbase;
	BreakerState_t m_state;
};

namespace features
{
	class c_ragebot {
	public:
		c_ragebot( );

		class c_lagcomp;
		struct lag_record_t;
		struct render_record_t;
		class c_antiaim;
		class c_lby_breaker;
		class c_resolver;

		std::shared_ptr< c_resolver > m_resolver;
		std::shared_ptr< c_lagcomp >  m_lagcomp;
		std::shared_ptr< c_antiaim >  m_antiaim;
	private:
		struct aim_target_t {
			int			  m_ent_index;
			int			  m_tick_count;
			int			  m_hitbox;
			vec3_t		  m_position;
			vec3_t		  m_min;
			vec3_t		  m_max;
			float		  m_radius;
			bool		  m_backtracked;
		};
		user_cmd_t* m_cmd;
		int			m_target;
		bool		m_send_next;
		matrix3x4   m_shot_matrix[ 128 ];
		int			m_shot_target;
		bool		m_can_fakeping;

		void run( );
		
		std::array< int, 65 > get_sorted_targets( );
		aim_target_t find_best_target( );
		int			 hitscan( c_base_player* ent, bool baim, vec3_t& out_pos, float& out_dmg );
		void		 set_angles( const vec3_t& viewangles );
		void		 aim_at_target( const aim_target_t& target );
		bool		 can_hit_without_ping( int entity );
		vec3_t		 multipoint( c_base_player* ent, int hitbox, float* out_dmg );
		int			 knifebot( );

		void update_settings( );
		bool should_baim_entity(c_base_player* ent, int weapon_damage, bool backtracked);
	public:
		int	 get_min_dmg( c_base_player* ent );
		bool can_fakeping( ) const { return m_can_fakeping; }
		auto get_shot_target( ) const { return m_shot_target; }
		auto get_shot_matrix( ) const { return m_shot_matrix; }
		int  get_target( ) const { return m_target; }
		void operator()( user_cmd_t* cmd );
	};

	class c_ragebot::c_lby_breaker {
		friend class c_antiaim;
	private:
		std::array< lby_update_t, 64 > m_lby_updates;
		void update_animstate( );

		float m_next_update{ };
		CCSGOPlayerAnimState* m_animstate{ };
	public:
		auto get_animstate() const { return m_animstate; }
		int  get_next_update( ) const;
		void on_lby_proxy( );
		void override_angles( float* angle, float original, float offset, bool freestanding = false );
	};

	class c_ragebot::c_antiaim {
	private:
		c_lby_breaker m_breaker;
		user_cmd_t* m_cmd;
		bool m_is_edging{ };
		bool m_is_fakewalking{ };
		bool m_direction{ };

		void run( );

		float get_yaw( bool fake, float original, bool no_jitter = false );
		float get_pitch( );

		bool run_freestanding( int player = -1 );
		bool run_edge_dtc( );

	public:
		bool is_fakewalking( ) const { return m_is_fakewalking; }
		bool is_edging( ) const { return m_is_edging; }
		float get_yaw( bool no_jitter = false );
		void run_fakewalk();
		auto get_animstate() const {
			return m_breaker.get_animstate();
		}

		void on_runcommand( ) {
			m_breaker.update_animstate( );
		}
		void on_lby_proxy( ) {
			m_breaker.on_lby_proxy( );
		}
		auto get_next_update( ) {
			return m_breaker.get_next_update( );
		}


		void fix_movement( );
		void run( user_cmd_t* cmd ) {
			m_cmd = cmd;
			run( );
		}
	};

	struct c_ragebot::lag_record_t {
		c_base_player* m_ent{ };

		int	m_tickcount{ };
		float m_flSimulationTime{ };
		float m_flOldSimulationTime{ };
		bool m_balanceadjust{ };
		bool m_shot{ };
		int m_choked{ };

		float m_flCycle{ };
		float m_flLowerBodyYawTarget{ };
		float m_flDuckAmount{ };
		float m_flDuckSpeed{ };
		float m_animDuck{ };
		int m_fFlags{ };
		int m_animFlags{ };
		float m_lastAnimTime;

		vec3_t m_animVelocity{ };
		vec3_t m_prevVelocity{ };

		bool m_sim_record{ };
		int m_shots{ };

		C_AnimationLayer m_AnimLayers[13]{ };
		float m_PoseParameters[24]{ };

		vec3_t m_vecOrigin{ }, m_vecAngles{ };
		vec3_t m_vecRenderOrigin{ }, m_vecRenderAngles{ };

		vec3_t m_vecMins{ }, m_vecMaxs{ };
		vec3_t m_vecHeadPos{ };
		vec3_t m_vecPelvisPos{ };

		CCSGOPlayerAnimState m_state{ };
		matrix3x4 m_matrix[128]{ };


		bool is_valid();

		lag_record_t() : m_tickcount(-1), m_valid(false) { }
		lag_record_t(c_base_player* ent);

		bool restore(bool recalculate = false, bool reanimate = false);

		bool m_valid{ };
	};

	struct c_ragebot::render_record_t {
		matrix3x4 m_matrix[ 128 ]{ };
		vec3_t	  m_origin{ };
		float	  m_simtime{ };
		float	  m_globaltime{ };
		float	  m_validtime{ };
	};

	class c_ragebot::c_lagcomp {
	public:
		using lag_deque_t = std::deque< lag_record_t >;
		using anim_array_t = C_AnimationLayer[ 13 ];
		using render_deque_t = std::deque< render_record_t >;


		lag_record_t* find_best_record( int ent_index );
		vec3_t		  get_backtracked_position( int ent_index );
		lag_deque_t*  get_records( int ent_index, RecordType_t type ) {
			switch( type ) {
			case RECORD_LBY:
				return &m_data_lby[ ent_index ];
			case RECORD_NORMAL:
				return &m_data_normal[ ent_index ];
			}

			return nullptr;
		}

		void store_record( int ent_index, RecordType_t type, const lag_record_t& record ) {
			auto records = get_records( ent_index, type );
			if( records ) {
				records->push_front( record );
			}
		}

		void	fsn_net_update_start( );
		void	fsn_render_start( );
		void	restore_animations( );
		void	store_visuals( );
		void	invalidate_bone_caches( );

		lag_record_t* get_newest_record( int ent_index );
		lag_record_t* get_newest_valid_record( int ent_index );

		bool get_render_record( int ent_index, matrix3x4* out, bool legit );

		void	operator()( user_cmd_t* cmd );

		int			 backtrack_entity( int ent_index, RecordType_t type, lag_record_t** out_record = nullptr );
		RecordType_t can_backtrack_entity( int ent_index );
		float		 get_flick_time( int ent_index ) {
			return m_flick_time[ ent_index ];
		}

		float&		 get_last_updated_simtime( int ent_index ) {
			return m_last_simtime[ ent_index ];
		}

	private:
		float		   m_last_simtime[ 65 ]{ };
		float		   m_flick_time[ 65 ]{ };
		bool		   m_first_update[ 65 ]{ };
		lag_deque_t    m_data_lby[ 65 ];
		lag_deque_t	   m_data_normal[ 65 ];
		render_deque_t m_data_render[ 65 ];
		user_cmd_t*	   m_cmd;
	};

	enum BreakingState_t {
		BREAKING_NONE,
		BREAKING,
		BREAKING_LAST,
	};

	enum OverrideDir_t {
		OVERRIDE_NONE,
		OVERRIDE_CENTER,
		OVERRIDE_LEFT,
		OVERRIDE_RIGHT,
		OVERRIDE_LAST
	};

	class c_ragebot::c_resolver {
	public:	
		void aimbot( int ent_index, int hitbox, vec3_t angle, vec3_t position, vec3_t min, vec3_t max, float radius );
		void listener( int ent_index, int shots );
		void frame_stage_notify( );
		std::vector< float > get_shot_vec( int ent );
		float get_shot_yaw( int shots, int ent );

		void force_yaw( c_base_player*, float );

		void brute_force( c_base_player* );
		int  manual_override( c_base_player* );
		void force_lby( c_base_player* );
		void on_missed_spread( int ent_index, int shots );
		int try_freestanding( c_base_player* );
		void update_player( int i );

		__forceinline bool is_breaking_lby( int ent_index ) {
			return !!m_data[ ent_index ].m_breaking;
		}

		__forceinline int get_breaking_state( int ent_index ) {
			return m_data[ ent_index ].m_breaking;
		}

		void increment_shots( int ent_index );

		__forceinline int get_shots( int ent_index ) {
			auto& data = m_data[ ent_index ];

			return data.m_missed_shots;
		}

		__forceinline bool& yaw_change( int ent_index ) {
			return m_data[ ent_index ].m_angle_change;
		}

	protected:

		struct resolve_log_t {
			//todo: deque to log hits etc
			void update( int ent_index );
			void reset( );

			bool		 m_logged{ };
			float		 m_logged_lby{ };
			int			 m_breaking{ };
			float		 m_last_lby{ };
			float		 m_last_moving_lby{ };
			float		 m_last_update{ };
			int			 m_shots{ };
			bool		 m_was_invalid{ };
			bool		 m_has_valid_lby{ };
			int			 m_last_freestanding{ };
			int			 m_overriding{ };
			int			 m_missed_shots{ };
			int			 m_logged_shots{ };
			bool		 m_angle_change{ };
			vec3_t		 m_last_pos{ };
		};

		struct resolve_data_t : public resolve_log_t {
			std::array< resolve_log_t, 150 > m_snapshot;
		};

		std::array< resolve_data_t, 65 > m_data;
	};

}
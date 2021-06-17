#pragma once
#include <vector>
#include <deque>

namespace features
{
	class c_extra {
		bool m_fire_next{ };

		// i'm gay
		struct netchan_record_t {
			float   m_curtime;
			int32_t m_reliable;
			int32_t m_sequence;
		};

		std::vector< netchan_record_t > m_net_records;

		int32_t m_last_sequence;

	public:
		void rank_reveal( user_cmd_t* ucmd );
		void thirdperson( );

		void update_netchannel( );
		void add_latency( INetChannel* channel );

		void float_ragdolls( );
		void rich_presence_flex( );
		void remove_rich_presence_flex( );

		void no_recoil( bool original );
		void no_recoil( user_cmd_t* cmd );
		void fake_duck( user_cmd_t* cmd );

		void no_smoke( );
		void no_flash( );

		void disable_post_processing( );

		void auto_revolver( user_cmd_t* );

		void money_talk( IGameEvent *evt );

		void unlock_hidden_cvars( );

	private:
		using grenade_trace_t = std::vector< vec3_t >;
		
	};
}
#include <algorithm>

#include "autowall.hpp"
#include "settings.hpp"
#include "context.hpp"
#include "math.hpp"



namespace features {
	bool c_autowall::is_armored( c_base_player* ent, int armor_value, int hitgroup ) {
		if ( armor_value <= 0 ) return false;

		switch ( hitgroup ) {
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			return true;
		case HITGROUP_HEAD:
			return ent->m_bHasHelmet( );
		}

		return false;
	}

	void c_autowall::trace_line( const vec3_t& start, const vec3_t& end, unsigned mask, void* ignore, CGameTrace* tr ) {
		CTraceFilter filter;
		Ray_t		 ray;

		filter.pSkip = ignore;
		ray.Init( start, end );

		g_csgo.m_trace( )->TraceRay( ray, mask, &filter, tr );
	}

	void c_autowall::scale_damage( c_base_player* ent, int hitgroup, float weapon_armor_ratio, float& damage ) {
		float multiplier;
		float armor_ratio;
		float new_damage;
		int armor;

		auto get_hitgroup_mul = []( int hitgroup ) {
			switch ( hitgroup ) {
			case HITGROUP_HEAD:
				return 4.f;
			case HITGROUP_STOMACH:
				return 1.25f;
			case HITGROUP_LEFTLEG:
			case HITGROUP_RIGHTLEG:
				return 0.75f;
			}

			return 1.0f;
		};

		multiplier = get_hitgroup_mul( hitgroup );
		damage *= multiplier;
		armor = ent->m_ArmorValue( );

		if ( is_armored( ent, armor, hitgroup ) ) {
			armor_ratio = weapon_armor_ratio * 0.5f;
			new_damage = damage * armor_ratio;
			if ( hitgroup != HITGROUP_HEAD && damage - damage * armor_ratio * 0.5f > armor ) {
				new_damage = damage - armor * 2.0f;
			}

			damage = new_damage;
		}
	}

	bool c_autowall::is_breakable( IClientEntity* ent ) {
	#ifdef is_breakable_ptr
		static auto is_breakable_ptr = g_header.patterns.is_breakable;
	#else
		static auto is_breakable_ptr = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "55 8B EC 51 56 8B F1 85 F6 74 68 83 BE" ) );
	#endif

		if ( !ent || ent->GetIndex( ) == 0 )
			return false;

		auto is_breakable_fn = reinterpret_cast< bool( __thiscall* )( void* ) >( is_breakable_ptr );

		bool breakable = is_breakable_fn( ent );

		if ( !breakable ) {
			auto ent_class = ent->GetClientClass( );

			if ( !ent_class )
				return false;

			// s/o 2 poiak
			auto nazwa = ent_class->m_name;

			// s/o 2 estrosterik
			if ( *reinterpret_cast< uint32_t* >( nazwa ) == 0x65724243 && *reinterpret_cast< uint32_t* >( nazwa + 0x7 ) == 0x53656C62 )
				return true;
		}

		return breakable;
	}

	bool c_autowall::trace_to_exit( vec3_t start, vec3_t& dir, vec3_t& out_end, CGameTrace& tr, CGameTrace* exit_trace ) {
		float dist = 0.f;
		int old_contents = 0;

		Ray_t ray;

		while ( dist <= 90.f ) {
			dist += 4.0f;

			out_end = start + ( dir * dist );

			if ( !old_contents )
				old_contents = g_csgo.m_trace( )->GetPointContents( out_end, MASK_SHOT_HULL | CONTENTS_HITBOX );

			int contents = g_csgo.m_trace( )->GetPointContents( out_end, MASK_SHOT_HULL | CONTENTS_HITBOX );
			if ( contents & MASK_SHOT_HULL && ( !( contents & CONTENTS_HITBOX ) || old_contents == contents ) )
				continue;

			vec3_t end = out_end - ( dir * 4.0f );

			ray.Init( out_end, end );

			g_csgo.m_trace( )->TraceRay( ray, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, exit_trace );

			if ( exit_trace->startsolid && exit_trace->surface.flags & SURF_HITBOX ) {
				CTraceFilter filter;
				filter.pSkip = exit_trace->m_pEnt;

				ray.Init( out_end, start );

				g_csgo.m_trace( )->TraceRay( ray, MASK_SHOT_HULL, &filter, exit_trace );

				if ( exit_trace->DidHit( ) && !exit_trace->startsolid ) {
					out_end = exit_trace->endpos;
					return true;
				}

				continue;
			}

			// maps/cs_office/glass/urban_glass
			const char* surface_name = tr.surface.name;

			if ( surface_name
				&& *( uint32_t* )( &surface_name[ 0 ] ) == 0x7370616d
				&& *( uint32_t* )( &surface_name[ 4 ] ) == 0x5f73632f
				&& *( uint32_t* )( &surface_name[ 8 ] ) == 0x6966666f
				&& *( uint32_t* )( &surface_name[ 12 ] ) == 0x672f6563
				&& *( uint32_t* )( &surface_name[ 16 ] ) == 0x7373616c
				&& *( uint32_t* )( &surface_name[ 20 ] ) == 0x6272752f
				&& *( uint32_t* )( &surface_name[ 24 ] ) == 0x675f6e61
				&& *( uint32_t* )( &surface_name[ 28 ] ) == 0x7373616c
				) {
				*exit_trace = tr;
				exit_trace->endpos = out_end + dir;
				return true;
			}

			if ( exit_trace->DidHit( ) && !exit_trace->startsolid ) {
				if ( is_breakable( tr.m_pEnt ) && is_breakable( tr.m_pEnt ) )
					return true;

				if ( tr.surface.flags & SURF_NODRAW || !( exit_trace->surface.flags & SURF_NODRAW ) && ( exit_trace->plane.normal.dot( dir ) <= 1.f ) ) {
					out_end -= dir * ( exit_trace->fraction * 4.f );
					return true;
				}

				continue;
			}

			if ( !exit_trace->DidHit( ) || exit_trace->startsolid ) {
				if ( tr.m_pEnt && tr.m_pEnt != g_csgo.m_entlist( )->GetClientEntity< void >( 0 ) && is_breakable( exit_trace->m_pEnt ) ) {
					*exit_trace = tr;
					exit_trace->endpos = out_end + dir;
					return true;
				}
			}
		}

		return false;
	}

	bool c_autowall::handle_bullet_penetration( weapon_info_t* wpn_data, fire_bullet_data_t& data ) {

		vec3_t     pen_end;
		CGameTrace exit_trace;



		if ( !trace_to_exit( data.enter_trace.endpos, data.direction, pen_end, data.enter_trace, &exit_trace ) ) {
			if( !( g_csgo.m_trace( )->GetPointContents( pen_end, MASK_SHOT_HULL ) & MASK_SHOT_HULL ) )
				return false;
		}

		bool is_light_surf = ( data.enter_trace.contents >> 7 ) & 1;
		bool is_solid_surf = ( data.enter_trace.contents >> 3 ) & 1;

		auto exit_surface = g_csgo.m_phys_props( )->GetSurfaceData( exit_trace.surface.surfaceProps );
		auto enter_surface = g_csgo.m_phys_props( )->GetSurfaceData( data.enter_trace.surface.surfaceProps );

		if ( !exit_surface || !enter_surface )
			return false;

		int exit_material = exit_surface->game.material, enter_material = enter_surface->game.material;

		auto damage_mod = 0.16f;
		auto pen_mod = ( enter_surface->game.penetrationmodifier + exit_surface->game.penetrationmodifier ) * 0.5f;

		constexpr uint32_t grate = 71, wood = 85, plastic = 76, glass = 89, cardboard = 87;

		if ( enter_material == grate /* metal vents */ || enter_material == glass /* windows */ ) {
			pen_mod = 3.0f;
			damage_mod = 0.05f;
		} else if ( is_light_surf || is_solid_surf ) {
			pen_mod = 1.0f;
			damage_mod = 0.16f;
		}

		if ( enter_material == exit_material ) {
			if ( exit_material == cardboard || exit_material == wood )
				pen_mod = 3.0f;
			else if ( exit_material == plastic )
				pen_mod = 2.0f;
		}

		float thickness = exit_trace.endpos.dist_to_sqr( data.enter_trace.endpos );

		auto modifier = math::max( 0.f, 1.0f / pen_mod );

		auto lost_dmg = std::fmaxf(
			( ( modifier * thickness ) / 24.f )
			+ ( ( data.current_damage * damage_mod )
			+ ( std::fmaxf( 3.75f / wpn_data->penetration, 0.f ) * 3.f * modifier ) ), 0.f );


		if ( lost_dmg > 0.f )
			data.current_damage -= lost_dmg;

		if ( data.current_damage < 1.f )
			return false;

		data.src = exit_trace.endpos;
		data.penetrate_count--;

		return true;
	}

	bool c_autowall::fire_bullet( c_base_player* shooter, c_base_player* target, weapon_info_t* wep_data, fire_bullet_data_t& data, bool ent_check, bool scale ) {
		data.penetrate_count = 4;

		if ( !wep_data )
			return false;

		data.current_damage = ( float )( wep_data->damage );

		while ( data.penetrate_count > 0 && data.current_damage >= 1.0f ) {
			// this little nigga all along.
			// never trust the feds.
			// or even fluffy people.
			data.to_travel = data.travel_range - data.traveled;
			vec3_t end = data.src + data.direction * data.to_travel;

			trace_line( data.src, end, MASK_SHOT | CONTENTS_GRATE, shooter, &data.enter_trace );
			util::clip_trace_to_player( target->ce( ), data.src, end + data.direction * 40.f, MASK_SHOT | CONTENTS_GRATE, &data.filter, &data.enter_trace );

			data.traveled += data.to_travel * data.enter_trace.fraction;
			data.current_damage *= std::pow( wep_data->range_modifier, data.traveled * 0.002f );

			if ( data.enter_trace.fraction == 1.0f ) {
				if ( !ent_check ) {
					if ( scale )
						scale_damage( target, HITGROUP_HEAD, wep_data->armor_ratio, data.current_damage );

					return true;
				}
				break;
			}


			if ( data.enter_trace.hitgroup <= HITGROUP_RIGHTLEG && data.enter_trace.hitgroup >= HITGROUP_HEAD
				&& data.enter_trace.m_pEnt == target->ce( ) ) {
				auto enemy = ( c_base_player* )( data.enter_trace.m_pEnt );

				if ( scale )
					scale_damage( enemy, data.enter_trace.hitgroup, wep_data->armor_ratio, data.current_damage );

				return true;
			}

			auto enter_surface = g_csgo.m_phys_props( )->GetSurfaceData( data.enter_trace.surface.surfaceProps );
			if ( data.traveled > 3000.f || enter_surface->game.penetrationmodifier < 0.1f )
				break;

			if ( !handle_bullet_penetration( wep_data, data ) )
				break;
		}

		return false;
	}

	float c_autowall::run( c_base_player* shooter, c_base_player* target, const vec3_t& end, bool ent_check ) {
		if ( !shooter || !target ) {
			return 0.f;
		}

		auto wep = shooter->get_weapon( );
		if ( !wep ) {
			return 0.f;
		}

		fire_bullet_data_t data;
		data.filter.pSkip = shooter;

		if ( shooter == g_ctx.m_local ) {
			data.src = shooter->get_eye_pos( );
		} else {
			data.src = shooter->m_vecOrigin( );
			data.src.z += 72.f;
		}

		data.direction = end - data.src;
		data.travel_range = data.direction.length( );
		data.direction.normalize_vector( );
		data.traveled = 0.f;
		data.to_travel = data.travel_range;


		if ( fire_bullet( shooter, target, wep->get_wpn_info( ), data, ent_check ) ) {
			return std::max( data.current_damage, 0.f );
		}

		return 0.f;
	}
}

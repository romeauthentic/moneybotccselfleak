#pragma once
#include "base_cheat.hpp"
#include "interface.hpp"

#include "js_mgr.h"
#include "ui.h"

#include "../internal_rewrite/context.hpp"

#define JS_FUNC( x ) static duk_ret_t x( duk_context* ctx )

namespace js {
namespace fn {
JS_FUNC( g_get_cur_time ) {
    duk_push_number( ctx, g_csgo.m_globals->m_curtime );
    return 1;
}

JS_FUNC( g_get_real_time ) {
    duk_push_number( ctx, g_csgo.m_globals->m_realtime );
    return 1;
}

JS_FUNC( g_get_frame_time ) {
    duk_push_number( ctx, g_csgo.m_globals->m_frametime );
    return 1;
}

JS_FUNC( g_get_max_players ) {
    duk_push_int( ctx, g_csgo.m_globals->m_maxclients );
    return 1;
}

JS_FUNC( g_get_tick_count ) {
	duk_push_int( ctx, g_csgo.m_globals->m_tickcount );
	return 1;
}

JS_FUNC( g_get_tick_interval ) {
	duk_push_number( ctx, g_csgo.m_globals->m_interval_per_tick );
	return 1;
}

JS_FUNC( g_get_frame_count ) {
	duk_push_number( ctx, g_csgo.m_globals->m_framecount );
	return 1;
}

JS_FUNC( g_get_max_entities ) {
	duk_push_int( ctx, g_csgo.m_entlist( )->GetHighestEntityIndex( ) );
	return 1;
}

JS_FUNC( g_get_frame_stage ) {
	duk_push_int( ctx, g_ctx.m_stage );
	return 1;
}

JS_FUNC( g_get_screen_size ) {	
	int wide, tall;
	g_csgo.m_surface.get( )->GetScreenSize( wide, tall );

	duk_idx_t arr_idx = duk_push_array( ctx );
	duk_push_number( ctx, wide );
	duk_put_prop_index( ctx, arr_idx, 0 );

	duk_push_number( ctx, tall );
	duk_put_prop_index( ctx, arr_idx, 1 );

	return 1;
}

JS_FUNC( g_get_key_pressed ) {
	duk_push_boolean( ctx, g_input.is_key_pressed( duk_to_int( ctx, 0 ) ) );
	return 1;
}

JS_FUNC( g_get_latency ) {
	duk_push_number( ctx, util::get_total_latency( ) );
	return 1;
}

JS_FUNC( g_exec ) {
	g_csgo.m_engine( )->ClientCmd_Unrestricted( duk_to_string( ctx, 0 ) );
	return 1;
}

JS_FUNC( g_get_cmd ) {
    user_cmd_t *cmd = g_ctx.get_last_cmd( );

	duk_idx_t arr_idx = duk_push_array( ctx );
    duk_push_number( ctx, cmd->m_cmd_nr ); 
	duk_put_prop_index( ctx, arr_idx, 0 ); 

	duk_push_number( ctx, cmd->m_forwardmove ); 
	duk_put_prop_index( ctx, arr_idx, 1 ); 

	duk_push_number( ctx, cmd->m_sidemove ); 
	duk_put_prop_index( ctx, arr_idx, 2 );

	duk_push_number( ctx, cmd->m_upmove ); 
	duk_put_prop_index( ctx, arr_idx, 3 );

    duk_push_number( ctx, cmd->m_buttons ); 
	duk_put_prop_index( ctx, arr_idx, 4 );

	return 1;
}

JS_FUNC( g_trace_ray ) {
	int skip_entity_idx = duk_to_int( ctx, 0 );

	CTraceFilter filter{ };
	CGameTrace	 tr{ };
	Ray_t		 ray{ };

    if( skip_entity_idx ) 
		filter.pSkip = g_csgo.m_entlist( )->GetClientEntity<>( skip_entity_idx );

	vec3_t src;
	vec3_t dst;

    // yep very epic
    // src
	duk_get_prop_index( ctx, 1, 0 );
	src.x = duk_to_number( ctx, -1 );
	duk_pop( ctx );

	duk_get_prop_index( ctx, 1, 1 );
	src.y = duk_to_number( ctx, -1 );
	duk_pop( ctx );

	duk_get_prop_index( ctx, 1, 2 );
	src.z = duk_to_number( ctx, -1 );
	duk_pop( ctx );

    // dst
	duk_get_prop_index( ctx, 2, 0 );
	dst.x = duk_to_number( ctx, -1 );
	duk_pop( ctx );

	duk_get_prop_index( ctx, 2, 1 );
	dst.y = duk_to_number( ctx, -1 );
	duk_pop( ctx );

	duk_get_prop_index( ctx, 2, 2 );
	dst.z = duk_to_number( ctx, -1 );
	duk_pop( ctx );

	ray.Init( src, dst );
	g_csgo.m_trace( )->TraceRay( ray, MASK_SHOT, &filter, &tr );

	duk_idx_t arr_idx = duk_push_array( ctx );
	duk_push_number( ctx, tr.fraction );
	duk_put_prop_index( ctx, arr_idx, 0 );

	duk_idx_t arr_idx2 = duk_push_array( ctx );
	duk_push_number( ctx, tr.endpos.x );
	duk_put_prop_index( ctx, arr_idx2, 0 );

	duk_push_number( ctx, tr.endpos.y );
	duk_put_prop_index( ctx, arr_idx2, 1 );

	duk_push_number( ctx, tr.endpos.z );
	duk_put_prop_index( ctx, arr_idx2, 2 );

	duk_put_prop_index( ctx, arr_idx, 1 ); 

	auto idx = tr.m_pEnt->as< c_base_player >( )->ce( )->GetIndex( );
	if( !idx ) idx = -1;

	duk_push_number( ctx, idx ); 
	duk_put_prop_index( ctx, arr_idx, 2 );

	return 1;
}

JS_FUNC( g_set_view_angles ) {
	vec3_t viewangles;

	duk_get_prop_index( ctx, 1, 0 );
	viewangles.x = duk_to_number( ctx, -1 );
	duk_pop( ctx );

	duk_get_prop_index( ctx, 1, 1 );
	viewangles.y = duk_to_number( ctx, -1 );
	duk_pop( ctx );

	duk_get_prop_index( ctx, 1, 2 );
	viewangles.z = duk_to_number( ctx, -1 );
	duk_pop( ctx );

	g_csgo.m_engine( )->SetViewAngles( viewangles );

	return 1;
}

JS_FUNC( g_get_view_angles ) {
	vec3_t viewangles;
	g_csgo.m_engine( )->GetViewAngles( viewangles );

	duk_idx_t arr_idx = duk_push_array( ctx ); 
	duk_push_number( ctx, viewangles.x ); 
	duk_put_prop_index( ctx, arr_idx, 0 ); 

	duk_push_number( ctx, viewangles.y ); 
	duk_put_prop_index( ctx, arr_idx, 1 );

	duk_push_number( ctx, viewangles.z ); 
	duk_put_prop_index( ctx, arr_idx, 2 );

	return 1;
}

JS_FUNC( g_print_to_chat ) {
    char print_str[ 256 ];

	sprintf_s( print_str, duk_to_string( ctx, 0 ) );
	g_csgo.m_clientmode( )->m_pChatElement->ChatPrintf( 0, 0, print_str );

	return 1;
}

JS_FUNC( g_log ) {
	g_con->log( duk_to_string( ctx, 0 ) );
	return 1;
}

JS_FUNC( e_get_all ) {
	duk_idx_t arr_idx = duk_push_array( ctx ); 

	for( int i = 0; i < g_csgo.m_entlist( )->GetHighestEntityIndex( ); ++i ) {
		auto ent = g_csgo.m_entlist( )->GetClientEntity<>( i );
		if( ent ) {
			int idx = ent->ce( )->GetIndex( );
			duk_push_int( ctx, idx );
			duk_put_prop_index( ctx, arr_idx, idx );
		}
	}

	return 1;
}

JS_FUNC( e_get_players ) {
	duk_idx_t  arr_idx = duk_push_array( ctx ); 

    for( size_t i{}; i < 65; ++i ) {
        auto ent = g_csgo.m_entlist( )->GetClientEntity<>( i );
        if( ent && ent->is_player( ) ) {
			int idx = ent->ce( )->GetIndex( );

            duk_push_int( ctx, idx );
            duk_put_prop_index( ctx, arr_idx, idx );
        }
    }

    return 1;
}

JS_FUNC( e_get_weapons ) {
	duk_idx_t arr_idx = duk_push_array( ctx ); 

	for( int i = 0; i < g_csgo.m_entlist( )->GetHighestEntityIndex( ); ++i ) {
		auto ent = g_csgo.m_entlist( )->GetClientEntity< IClientEntity >( i );
		if( ent && ent->as< c_base_weapon >( )->is_weapon( ) ) {
			int idx = ent->GetIndex( );

			duk_push_int( ctx, idx );
			duk_put_prop_index( ctx, arr_idx, idx );
		}
	}

	return 1;
}

JS_FUNC( e_get_local_player ) {
	duk_push_number( ctx, g_ctx.m_local->ce( )->GetIndex( ) );
	return 1;
}

JS_FUNC( e_is_enemy ) {
	int index = duk_to_int( ctx, 0 );
	auto ent = g_csgo.m_entlist( )->GetClientEntity<>( index );
	if( ent ) {
		duk_push_boolean( ctx, ent->m_iTeamNum( ) != g_ctx.m_local->m_iTeamNum( ) );
		return 1;
	}

	duk_push_false( ctx );
	return 1;
}

JS_FUNC( e_is_dormant ) {
	int index = duk_to_int( ctx, 0 );
	auto ent = g_csgo.m_entlist( )->GetClientEntity<>( index );
	if( ent ) {
	    duk_push_boolean( ctx, ent->ce( )->IsDormant( ) );
	    return 1;
	}

	duk_push_false( ctx );
	return 1;
}

JS_FUNC( e_is_alive ) {
    int index = duk_to_int( ctx, 0 );
    auto ent = g_csgo.m_entlist( )->GetClientEntity<>( index );
    if( ent && ent->is_player( ) ) {
        duk_push_boolean( ctx, ent->is_alive( ) );
        return 1;
    }

    duk_push_false( ctx );
    return 1;
}

JS_FUNC( e_is_valid ) {
	int index = duk_to_int( ctx, 0 );
	auto ent = g_csgo.m_entlist( )->GetClientEntity<>( index );
	ent ? duk_push_true( ctx ) : duk_push_false( ctx );
	
	return 1;
}

JS_FUNC( e_get_name ) {
    int index = duk_to_int( ctx, 0 );
    auto ent = g_csgo.m_entlist( )->GetClientEntity<>( index );
    if( ent && ent->is_player( ) ) {
        char name[ 32 ];
        ent->get_name_safe( name );

        duk_push_string( ctx, name );
        return 1;
    }

    return 1;
}

JS_FUNC( e_get_hitbox_pos ) {
	int index = duk_to_int( ctx, 0 );
	int hitbox = duk_to_int( ctx, 1 );
	auto ent = g_csgo.m_entlist( )->GetClientEntity<>( index );
	if( ent && ent->is_player( ) ) {
		vec3_t hitbox_pos = ent->get_hitbox_pos( hitbox );

		duk_idx_t arr_idx = duk_push_array( ctx ); 
		duk_push_number( ctx, hitbox_pos.x ); 
		duk_put_prop_index( ctx, arr_idx, 0 ); 

		duk_push_number( ctx, hitbox_pos.y ); 
		duk_put_prop_index( ctx, arr_idx, 1 );

		duk_push_number( ctx, hitbox_pos.z ); 
		duk_put_prop_index( ctx, arr_idx, 2 );

		return 1;
	}

	return 1;
}

JS_FUNC( e_get_steam64 ) {
	int index = duk_to_int( ctx, 0 );
	auto ent = g_csgo.m_entlist( )->GetClientEntity<>( index );
	if( ent && ent->is_player( ) ) {
		duk_push_string( ctx, ent->get_info( ).m_steamid );
		return 1;
	}

	return 1;
}

JS_FUNC( e_get_ping ) {
	int index = duk_to_int( ctx, 0 );
	auto ent = g_csgo.m_entlist( )->GetClientEntity<>( index );
	if( ent && ent->is_player( ) ) {
		duk_push_number( ctx, ent->get_ping( ) );
		return 1;
	}

	return 1;
}

JS_FUNC( e_get_prop ) {
	int        index = duk_to_int( ctx, 0 );
	const char *table = duk_to_string( ctx, 1 );
	const char *prop = duk_to_string( ctx, 2 );

	auto starts_with = []( const std::string& str, const std::string& match ) {
		return str.find( match ) == 0;
	};

	auto ent = g_csgo.m_entlist( )->GetClientEntity<>( index );
	if( ent ) {
		auto offset = g_netvars.get_netvar( hash::fnv1a( table ), hash::fnv1a( prop ) );

        // return the correct type (GAY)
		if( starts_with( prop, xors( "m_b" ) ) ) {
			duk_push_boolean( ctx, ent->get< bool >( offset ) );
			return 1;
		}

		if( starts_with( prop, xors( "m_fl" ) ) ) {
			duk_push_number( ctx, ent->get< float >( offset ) );
			return 1;
		}

		if( starts_with( prop, xors( "m_i" ) ) || starts_with( prop, xors( "m_n" ) ) || starts_with( prop, xors( "m_f" ) ) ) {
			duk_push_number( ctx, ent->get< int >( offset ) );
			return 1;
		}

		if( starts_with( prop, xors( "m_vec" ) ) 
			|| starts_with( prop, xors( "m_ang" ) ) 
			|| starts_with( prop, xors( "m_aim" ) ) 
			|| starts_with( prop, xors( "m_view" ) ) ) { // valve is not consistent :)

			vec3_t vector = ent->get< vec3_t >( offset ); 

			duk_idx_t arr_idx = duk_push_array( ctx ); 
			duk_push_number( ctx, vector.x ); 
			duk_put_prop_index( ctx, arr_idx, 0 ); 

			duk_push_number( ctx, vector.y ); 
			duk_put_prop_index( ctx, arr_idx, 1 ); 

			duk_push_number( ctx, vector.z ); 
			duk_put_prop_index( ctx, arr_idx, 2 ); 

			return 1; 
		}

		if( starts_with( prop, xors( "m_sz" ) ) ) {
            const char *meme = ent->get_str( offset );
			duk_push_string( ctx, meme );
			return 1;
		}

		if( starts_with( prop, xors( "m_h" ) ) ) {
			auto handle = g_csgo.m_entlist( )->GetClientEntityFromHandle( ent->get< uint32_t >( offset ) );
            if( handle ) {
				duk_push_number( ctx, handle->ce( )->GetIndex( ) );
            }
			return 1;
		}

        // other types can go fuck themselves
		return 1;
	}

	return 1;
}

JS_FUNC( e_set_prop ) {
	int        index = duk_to_int( ctx, 0 );
	const char *table = duk_to_string( ctx, 1 );
	const char *prop = duk_to_string( ctx, 2 );

	auto starts_with = []( const std::string& str, const std::string& match ) {
		return str.find( match ) == 0;
	};

	auto ent = g_csgo.m_entlist( )->GetClientEntity<>( index );
	if( ent ) {
		if( starts_with( prop, xors( "m_b" ) ) ) {
			auto offset = g_netvars.get_netvar( hash::fnv1a( table ), hash::fnv1a( prop ) );
			*reinterpret_cast< bool* >( uintptr_t( ent ) + offset ) = duk_to_boolean( ctx, 3 );
			return 1;
		}

		if( starts_with( prop, xors( "m_fl" ) ) ) {
			auto offset = g_netvars.get_netvar( hash::fnv1a( table ), hash::fnv1a( prop ) );
			*reinterpret_cast< float* >( uintptr_t( ent ) + offset ) = duk_to_number( ctx, 3 );
			return 1;
		}

		if( starts_with( prop, xors( "m_i" ) ) || starts_with( prop, xors( "m_n" ) ) || starts_with( prop, xors( "m_f" ) ) ) {
			auto offset = g_netvars.get_netvar( hash::fnv1a( table ), hash::fnv1a( prop ) );
			*reinterpret_cast< int* >( uintptr_t( ent ) + offset ) = duk_to_int( ctx, 3 );
			return 1;
		}

		if( starts_with( prop, xors( "m_vec" ) ) 
			|| starts_with( prop, xors( "m_ang" ) ) 
			|| starts_with( prop, xors( "m_aim" ) ) 
			|| starts_with( prop, xors( "m_view" ) ) ) { // valve is not consistent :)
			auto offset = g_netvars.get_netvar( hash::fnv1a( table ), hash::fnv1a( prop ) );

			vec3_t vec;

			duk_get_prop_index( ctx, -1, 0 );
			vec.x = duk_to_int( ctx, -1 );
			duk_pop( ctx );

			duk_get_prop_index( ctx, -1, 1 );
			vec.y = duk_to_int( ctx, -1 );
			duk_pop( ctx );

			duk_get_prop_index( ctx, -1, 2 );
			vec.z = duk_to_int( ctx, -1 );
			duk_pop( ctx );
			
			*reinterpret_cast< vec3_t* >( uintptr_t( ent ) + offset ) = vec;

			return 1;
		}

		return 1;
	}

	return 1;
}

JS_FUNC( e_get_class_id ) {
	int index = duk_to_int( ctx, 0 );
	auto ent = g_csgo.m_entlist( )->GetClientEntity<>( index );
	if( ent ) {
		auto clientclass = ent->ce( )->GetClientClass( );
		if( clientclass ) {
			duk_push_number( ctx, clientclass->m_class_id );
			return 1;
		}
	}

	return 1;
}

JS_FUNC( e_get_class_name ) {
	int index = duk_to_int( ctx, 0 );
	auto ent = g_csgo.m_entlist( )->GetClientEntity<>( index );
	if( ent ) {
		auto clientclass = ent->ce( )->GetClientClass( );
		if( clientclass ) {
			duk_push_string( ctx, clientclass->m_name );
			return 1;
		}
	}

	return 1;
}

JS_FUNC( e_get_eye_pos ) {
	int index = duk_to_int( ctx, 0 );
	auto ent = g_csgo.m_entlist( )->GetClientEntity<>( index );
	if( ent && ent->is_player( ) ) {
		vec3_t eye_pos = ent->get_eye_pos( );

		duk_idx_t arr_idx = duk_push_array( ctx );
		duk_push_number( ctx, eye_pos.x );
		duk_put_prop_index( ctx, arr_idx, 0 );

		duk_push_number( ctx, eye_pos.y );
		duk_put_prop_index( ctx, arr_idx, 1 );

		duk_push_number( ctx, eye_pos.z );
		duk_put_prop_index( ctx, arr_idx, 2 );
		return 1;
	}

	return 1;
}



JS_FUNC( e_get_desync_delta ) {
	int index = duk_to_int( ctx, 0 );
	auto ent = g_csgo.m_entlist( )->GetClientEntity<>( index );
	if( ent && ent->is_player( ) && ent->has_valid_anim( ) ) {		
		duk_push_number( ctx, ent->get_animstate( )->desync_delta( ) );
		return 1;
	}

	return 1;
}


JS_FUNC( cvar_get_int ) {
	const char* str = duk_to_string( ctx, 0 );
	duk_push_int( ctx, g_csgo.m_cvar( )->FindVar( str )->get_int( ) );

	return 1;
}

JS_FUNC( cvar_set_int ) {
	const char* str = duk_to_string( ctx, 0 );
	int         value = duk_to_int( ctx, 1 );

	auto var = g_csgo.m_cvar( )->FindVar( str );
	var->m_flags |= 0;
	var->set_value( value );

	return 1;
}

JS_FUNC( cvar_get_float ) {
	const char* str = duk_to_string( ctx, 0 );
	duk_push_number( ctx, g_csgo.m_cvar( )->FindVar( str )->get_float( ) );

	return 1;
}

JS_FUNC( cvar_set_float ) {
	const char* str = duk_to_string( ctx, 0 );
	float       value = duk_to_number( ctx, 1 );

	auto var = g_csgo.m_cvar( )->FindVar( str );
	var->m_flags |= 0;
	var->set_value( value );

	return 1;
}

JS_FUNC( cvar_get_string ) {
	const char* str = duk_to_string( ctx, 0 );
	duk_push_string( ctx, g_csgo.m_cvar( )->FindVar( str )->get_string( ) );

	return 1;
}

JS_FUNC( cvar_set_string ) {
	const char* str = duk_to_string( ctx, 0 );
	const char* value = duk_to_string( ctx, 1 );

	auto var = g_csgo.m_cvar( )->FindVar( str );
	var->m_flags |= 0;
	var->set_value( value );

	return 1;
}

JS_FUNC( v_draw_string ) {
    int         x = duk_to_int( ctx, 0 );
    int         y = duk_to_int( ctx, 1 );
    int         a = duk_to_int( ctx, 2 );
    const char* str = duk_to_string( ctx, 3 );

	clr_t clr = clr_t( 255, 255, 255, 255 );
	bool once = false;
	if( !once ){
		once = !once;
		clr = g_js->m_clr;
	}

    g_cheat.m_visuals.draw_string( x, y, a, false, clr, str );

    return 0;
}

JS_FUNC( v_draw_line ) {
    int x1 = duk_to_int( ctx, 0 );
    int y1 = duk_to_int( ctx, 1 );
    int x2 = duk_to_int( ctx, 2 );
    int y2 = duk_to_int( ctx, 3 );

	clr_t clr = clr_t( 255, 255, 255, 255 );
	bool once = false;
	if( !once ){
		once = !once;
		clr = g_js->m_clr;
	}

    g_cheat.m_visuals.draw_line( x1, y1, x2, y2, clr );

    return 0;
}

JS_FUNC( v_draw_rect ) {
    int x = duk_to_int( ctx, 0 );
    int y = duk_to_int( ctx, 1 );
    int w = duk_to_int( ctx, 2 );
    int h = duk_to_int( ctx, 3 );

	clr_t clr = clr_t( 255, 255, 255, 255 );
	bool once = false;
	if( !once ){
		once = !once;
		clr = g_js->m_clr;
	}

    g_cheat.m_visuals.draw_rect( x, y, w, h, clr );

    return 0;
}

JS_FUNC( v_draw_filled_rect ) {
    int x = duk_to_int( ctx, 0 );
    int y = duk_to_int( ctx, 1 );
    int w = duk_to_int( ctx, 2 );
    int h = duk_to_int( ctx, 3 );

	clr_t clr = clr_t( 255, 255, 255, 255 );
	bool once = false;
	if( !once ){
		once = !once;
		clr = g_js->m_clr;
	}

    g_cheat.m_visuals.draw_filled_rect( x, y, w, h, clr );

    return 0;
}

JS_FUNC( v_draw_circle ) {
    int x = duk_to_int( ctx, 0 );
    int y = duk_to_int( ctx, 1 );
    int r = duk_to_int( ctx, 2 );

	clr_t clr = clr_t( 255, 255, 255, 255 );
	bool once = false;
	if( !once ){
		once = !once;
		clr = g_js->m_clr;
	}

    g_cheat.m_visuals.draw_circle( x, y, r, clr );

    return 0;
}

JS_FUNC( v_set_clr ) {
	int r = duk_to_int( ctx, 0 );
	int g = duk_to_int( ctx, 1 );
	int b = duk_to_int( ctx, 2 );
	int a = duk_to_int( ctx, 3 );

	g_js->m_clr = clr_t( r, g, b, a );

	return 0;
}

JS_FUNC( v_world_to_screen ) {
	vec3_t vec;

	duk_get_prop_index( ctx, -1, 0 );
	vec.x = duk_to_int( ctx, -1 );
	duk_pop( ctx );

	duk_get_prop_index( ctx, -1, 1 );
	vec.y = duk_to_int( ctx, -1 );
	duk_pop( ctx );

	duk_get_prop_index( ctx, -1, 2 );
	vec.z = duk_to_int( ctx, -1 );
	duk_pop( ctx );

	vec2_t screen = util::screen_transform( vec );

	duk_idx_t arr_idx = duk_push_array( ctx );
	duk_push_number( ctx, screen.x );
	duk_put_prop_index( ctx, arr_idx, 0 );

	duk_push_number( ctx, screen.y );
	duk_put_prop_index( ctx, arr_idx, 1 );
	return 1;
}

JS_FUNC( ui_get_value ) {
    std::shared_ptr< ui::base_item > item = ui::menu;

    auto varcount = duk_get_top( ctx );
    if( !varcount ) {
        duk_push_number( ctx, 0 );
        return 1;
    }

    for( size_t i = 0; i < varcount; ++i ) {
        auto str = duk_to_string( ctx, i );
        item = item->find_item( str );
    }

    duk_push_number( ctx, item->get_value( ) );
    return 1;
}

JS_FUNC( ui_set_value ) {
    std::shared_ptr< ui::base_item > item = ui::menu;

    auto varcount = duk_get_top( ctx );
    if( varcount < 2 )
        return 0;

    for( size_t i = 0; i < varcount - 1; ++i ) {
        auto str = duk_to_string( ctx, i );
        item = item->find_item( str );
    }

    auto val = duk_to_number( ctx, varcount - 1 );
    item->set_value( val );

    return 0;
}

JS_FUNC( ui_add_checkbox ) {
    std::shared_ptr< ui::base_item > item = ui::menu;

    auto varcount = duk_get_top( ctx );
    if( varcount < 2 )
        return 0;

    for( size_t i = 0; i < varcount - 1; ++i ) {
        auto str = duk_to_string( ctx, i );
        item = item->find_item( str );
    }

    auto name = duk_to_string( ctx, varcount - 1 );

    auto setting = new con_var< bool >( &data::holder_, hash::fnv1a( name ) );
    item->add_item( std::make_shared< ui::c_checkbox >( 0, 0, name, &( *setting ) ) );

    return 0;
}

JS_FUNC( ui_add_slider_int ) {
	std::shared_ptr< ui::base_item > item = ui::menu;

	auto varcount = duk_get_top( ctx );
	if( varcount < 2 )
		return 0;

	for( size_t i = 0; i < varcount - 1; ++i ) {
		auto str = duk_to_string( ctx, i );
		item = item->find_item( str );
	}

	auto name = duk_to_string( ctx, varcount - 3 );
	auto min = duk_to_int( ctx, varcount - 2 );
	auto max = duk_to_int( ctx, varcount - 1 );

	auto setting = new con_var< int >( &data::holder_, hash::fnv1a( name ) );
	item->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, min, max, name, &( *setting ) ) );

	return 0;
}

JS_FUNC( ui_add_slider_float ) {
	std::shared_ptr< ui::base_item > item = ui::menu;

	auto varcount = duk_get_top( ctx );
	if( varcount < 2 )
		return 0;

	for( size_t i = 0; i < varcount - 1; ++i ) {
		auto str = duk_to_string( ctx, i );
		item = item->find_item( str );
	}

	auto name = duk_to_string( ctx, varcount - 3 );
	auto min = duk_to_number( ctx, varcount - 2 );
	auto max = duk_to_number( ctx, varcount - 1 );

	auto setting = new con_var< float >( &data::holder_, hash::fnv1a( name ) );
	item->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, min, max, name, &( *setting ) ) );

	return 0;
}

JS_FUNC( ui_add_hotkey ) {
	std::shared_ptr< ui::base_item > item = ui::menu;

	auto varcount = duk_get_top( ctx );
	if( varcount < 2 )
		return 0;

	for( size_t i = 0; i < varcount - 1; ++i ) {
		auto str = duk_to_string( ctx, i );
		item = item->find_item( str );
	}

	auto name = duk_to_string( ctx, varcount - 1 );

	auto setting = new con_var< int >( &data::holder_, hash::fnv1a( name ) );
	item->add_item( std::make_shared< ui::c_key_picker_small >( 195, 0, &( *setting ), name ) );

	return 0;
}

class c_dropdown_obj {
public:
	std::vector< ui::dropdowns::dropdown_item_t< > > items{};
	int item_count = 0;
};

JS_FUNC( ui_add_dropdown ) {
	std::shared_ptr< ui::base_item > item = ui::menu;

	auto varcount = duk_get_top( ctx );
	if( varcount < 2 )
		return 0;

	for( size_t i = 0; i < varcount - 1; ++i ) {
		auto str = duk_to_string( ctx, i );
		item = item->find_item( str );
	}

	std::unique_ptr< c_dropdown_obj > dropdown_obj = std::make_unique< c_dropdown_obj >();

	auto arr_len = duk_get_length( ctx, -1 );
	for( size_t i = 0; i < arr_len; i++ ) {
		duk_get_prop_index( ctx, -1, i );
		dropdown_obj->items.push_back( { duk_to_string( ctx, -1 ), dropdown_obj->item_count++ } );
		duk_pop( ctx );
	}

	auto name = duk_to_string( ctx, varcount - 2 );

	auto setting = new con_var< int >( &data::holder_, hash::fnv1a( name ) );
	setting->set( 0 ); // set a default value
	item->add_item( std::make_shared< ui::c_dropdown< int > >( 0, 0, 120, name, &( *setting ), &dropdown_obj->items ) );

	dropdown_obj.release( );

	return 0;
}

JS_FUNC( ui_add_form ) {
	std::shared_ptr< ui::base_item > item = ui::menu;

	auto varcount = duk_get_top( ctx );
	if( varcount < 2 )
		return 0;

	for( size_t i = 0; i < varcount - 1; ++i ) {
		auto str = duk_to_string( ctx, i );
		item = item->find_item( str );
	}

	auto name = duk_to_string( ctx, varcount - 3 );
	auto width = duk_to_int( ctx, varcount - 2 );
	auto max_height = duk_to_int( ctx, varcount - 1 );

	item->add_item( std::make_shared< ui::c_form >( 0, 0, width, max_height, name, false, true ) );

	return 0;
}

JS_FUNC( ui_set_visible ) {
	std::shared_ptr< ui::base_item > item = ui::menu;

	auto varcount = duk_get_top( ctx );
	if( varcount < 2 )
		return 0;

	for( size_t i = 0; i < varcount - 1; ++i ) {
		auto str = duk_to_string( ctx, i );
		item = item->find_item( str );
	}

	item->set_visible( duk_to_boolean( ctx, varcount - 1 ) );

	return 0;
}

JS_FUNC( ui_is_menu_open ) {
	duk_push_boolean( ctx, g_settings.menu.open );
	return 0;
}

JS_FUNC( add_on_draw ) {
	g_js->add_on_draw( duk_to_string( ctx, 0 ) );
	return 0;
}

JS_FUNC( add_on_stage ) {
	g_js->add_on_stage( duk_to_string( ctx, 0 ) );
	return 0;
}

JS_FUNC( add_on_cmove ) {
	g_js->add_on_cmove( duk_to_string( ctx, 0 ) );
	return 0;
}

JS_FUNC( add_on_event ) {
	g_js->add_on_cmove( duk_to_string( ctx, 0 ) );
	return 0;
}

}
}

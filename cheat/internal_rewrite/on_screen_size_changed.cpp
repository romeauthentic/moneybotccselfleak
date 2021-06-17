#include "hooks.hpp"

void __fastcall hooks::on_screen_size_changed( void* ecx_, void* edx_, int old_w, int old_h ) {
	static auto old_fn = on_screen_size_changed_o;

	old_fn( ecx_, nullptr, old_w, old_h );

	g_fonts.initialize( );
}
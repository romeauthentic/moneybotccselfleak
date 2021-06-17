#include "c_base_weapon.hpp"
#include "base_cheat.hpp"

char c_base_weapon::get_hud_icon( ) {
	if( is_knife( ) ) return 'J';

	switch( m_iItemDefinitionIndex( ) ) {
	case WEAPON_DEAGLE: return 'F';
	case WEAPON_ELITE: return 'S';
	case WEAPON_FIVESEVEN: return 'U';
	case WEAPON_GLOCK: return 'C';
	case WEAPON_AK47: return 'B';
	case WEAPON_AUG: return 'E';
	case WEAPON_AWP: return 'R';
	case WEAPON_FAMAS: return 'T';
	case WEAPON_G3SG1: return 'I';
	case WEAPON_GALIL: return 'V';
	case WEAPON_M249: return 'Z';
	case WEAPON_M4A4:
	case WEAPON_M4A1S:
		return 'W';
	case WEAPON_MAC10: return 'L';
	case WEAPON_P90: return 'M';
	case WEAPON_UMP45: return 'Q';
	case WEAPON_XM1014: return ']';
	case WEAPON_BIZON: return 'D';
	case WEAPON_MAG7: return 'K';
	case WEAPON_NEGEV: return 'Z';
	case WEAPON_SAWEDOFF: return 'K';
	case WEAPON_TEC9: return 'C';
	case WEAPON_ZEUS: return 'Y';
	case WEAPON_P2000: return 'Y';
	case WEAPON_MP7: return 'X';
	case WEAPON_MP9: return 'D';
	case WEAPON_NOVA: return 'K';
	case WEAPON_P250: return 'Y';
	case WEAPON_SCAR20: return 'I';
	case WEAPON_SG553: return '[';
	case WEAPON_SSG08: return 'N';
	case WEAPON_FLASHBANG: return 'G';
	case WEAPON_HEGRENADE: return 'H';
	case WEAPON_SMOKEGRENADE: return 'P';
	case WEAPON_MOLOTOV: return 'H';
	case WEAPON_DECOY: return 'G';
	case WEAPON_INC: return 'H';
	case WEAPON_C4: return '\\';
	case WEAPON_USPS: return 'Y';
	case WEAPON_CZ75: return 'C';
	case WEAPON_REVOLVER: return 'F';
	default: return 'J';
	}
}

int c_base_weapon::get_seq_activity( int sequence ) {
	static uintptr_t fn_offset = pattern::first_code_match(
		g_csgo.m_chl.dll( ), xors( "55 8B EC 83 7D 08 FF 56 8B F1 74 3D" ) );
	auto fn = ( int( __thiscall* )( void*, int ) )( fn_offset );

	return fn ? fn( this, sequence ) : -2;
}
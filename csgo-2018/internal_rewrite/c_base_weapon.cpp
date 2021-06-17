#include "c_base_weapon.hpp"

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
	case WEAPON_GALILAR: return 'V';
	case WEAPON_M249: return 'Z';
	case WEAPON_M4A1:
	case WEAPON_M4A1_SILENCER:
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
	case WEAPON_TASER: return 'Y';
	case WEAPON_HKP2000: return 'Y';
	case WEAPON_MP7: return 'X';
	case WEAPON_MP9: return 'D';
	case WEAPON_NOVA: return 'K';
	case WEAPON_P250: return 'Y';
	case WEAPON_SCAR20: return 'I';
	case WEAPON_SG556: return '[';
	case WEAPON_SSG08: return 'N';
	case WEAPON_FLASHBANG: return 'G';
	case WEAPON_HEGRENADE: return 'H';
	case WEAPON_SMOKEGRENADE: return 'P';
	case WEAPON_MOLOTOV: return 'H';
	case WEAPON_DECOY: return 'G';
	case WEAPON_INCGRENADE: return 'H';
	case WEAPON_C4: return '\\';
	case WEAPON_USP_SILENCER: return 'Y';
	case WEAPON_CZ75A: return 'C';
	case WEAPON_R8REVOLVER: return 'F';
	default: return 'J';
	}
}
#pragma once
#include "IClientEntity.h"
#include "CAttribute.h"
#include "netvars.h"
enum class weapon_attributes {
	weapon_allow_inspect = 731,
};

enum scoutweapons
{
	//Primary
	WPN_Scattergun = 13,
	WPN_NewScattergun = 200,
	WPN_FAN = 45,
	WPN_Shortstop = 220,
	WPN_SodaPopper = 448,
	WPN_BabyFaceBlaster = 772,
	WPN_BotScattergunS = 799,
	WPN_BotScattergunG = 808,
	WPN_BotScattergunR = 888,
	WPN_BotScattergunB = 897,
	WPN_BotScattergunC = 906,
	WPN_BotScattergunD = 915,
	WPN_BotScattergunES = 964,
	WPN_BotScattergunEG = 973,
	WPN_FestiveFaN = 1078,
	WPN_BackScatter = 1103,
	//Secondary
	WPN_ScoutPistol = 23,
	WPN_NewPistol = 209,
	WPN_Bonk = 46,
	WPN_CritCola = 163,
	WPN_Lugermorph = 160,
	WPN_Milk = 222,
	WPN_Lugermorph2 = 294,
	WPN_Winger = 449,
	WPN_PocketPistol = 773,
	WPN_MutatedMilk = 1121,
	WPN_CAPPER = 30666,
	//Melee
	WPN_Bat = 0,
	WPN_NewBat = 190,
	WPN_Sandman = 44,
	WPN_Fish = 221,
	WPN_Cane = 317,
	WPN_BostonBasher = 325,
	WPN_SunStick = 349,
	WPN_FanOWar = 355,
	WPN_RuneBlade = 452,
	WPN_Saxxy = 423,
	WPN_Atomizer = 450,
	WPN_ConscientiousObjector = 474,
	WPN_UnarmedCombat = 572,
	WPN_WrapAssassin = 648,
	WPN_FestiveBat = 660,
	WPN_FestiveScattergun = 669,
	WPN_FlyingGuillotine1 = 812,
	WPN_FlyingGuillotine2 = 833,
	WPN_FreedomStaff = 880,
	WPN_BatOuttaHell = 939,
	WPN_MemoryMaker = 954,
	WPN_FestiveFish = 999,
	WPN_TheHamShank = 1013,
	WPN_CrossingGuard = 1127,
	WPN_NecroSmasher = 1123,
	WPN_Batsaber = 30667,
};
enum soldierweapons
{
	//Primary
	WPN_RocketLauncher = 18,
	WPN_NewRocketLauncher = 205,
	WPN_DirectHit = 127,
	WPN_BlackBox = 228,
	WPN_RocketJumper = 237,
	WPN_LibertyLauncher = 414,
	WPN_CowMangler = 441,
	WPN_Original = 513,
	WPN_FestiveRocketLauncher = 658,
	WPN_BeggersBazooka = 730,
	WPN_BotRocketlauncherS = 800,
	WPN_BotRocketlauncherG = 809,
	WPN_BotRocketlauncherR = 889,
	WPN_BotRocketlauncherB = 898,
	WPN_BotRocketlauncherC = 907,
	WPN_BotRocketlauncherD = 916,
	WPN_BotRocketlauncherES = 965,
	WPN_BotRocketlauncherEG = 974,
	WPN_FestiveBlackbox = 1085,
	WPN_Airstrike = 1104,
	//Secondary
	WPN_SoldierShotgun = 10,
	WPN_NewShotgun = 199,
	WPN_BuffBanner = 129,
	WPN_BattalionBackup = 226,
	WPN_Concheror = 354,
	WPN_ReserveShooter = 415,
	WPN_RighteousBison = 442,
	WPN_FestiveBuffBanner = 1001,
	WPN_PanicAttack = 1153,
	WPN_ManTreads = 444,
	WPN_GunBoats = 133,
	WPN_BASEJumper = 1101,
	//Melee
	WPN_Shovel = 6,
	WPN_NewShovel = 196,
	WPN_Equalizer = 128,
	WPN_PainTrain = 154,
	WPN_Katana = 357,
	WPN_MarketGardener = 416,
	WPN_DisciplinaryAction = 447,
	WPN_EscapePlan = 775,
};
enum pyroweapons
{
	//Primary
	WPN_Flamethrower = 21,
	WPN_NewFlamethrower = 208,
	WPN_Backburner = 40,
	WPN_Degreaser = 215,
	WPN_FestiveFlamethrower = 659,
	WPN_Phlogistinator = 594,
	WPN_Rainblower = 741,
	WPN_BotFlamethrowerS = 798,
	WPN_BotFlamethrowerG = 807,
	WPN_BotFlamethrowerR = 887,
	WPN_BotFlamethrowerB = 896,
	WPN_BotFlamethrowerC = 905,
	WPN_BotFlamethrowerD = 914,
	WPN_BotFlamethrowerES = 963,
	WPN_BotFlamethrowerEG = 972,
	WPN_FestiveBackburner = 1146,
	//Secondary
	WPN_PyroShotgun = 12,
	WPN_Flaregun = 39,
	WPN_Detonator = 351,
	WPN_ManMelter = 595,
	WPN_ScorchShot = 740,
	WPN_FestiveFlaregun = 1081,
	WPN_ThermalThruster = 1179,
	WPN_GasPasser = 1180,
	//Melee
	WPN_Fireaxe = 2,
	WPN_NewAxe = 192,
	WPN_Axtingusher = 38,
	WPN_HomeWrecker = 153,
	WPN_PowerJack = 214,
	WPN_Backscratcher = 326,
	WPN_VolcanoFragment = 348,
	WPN_Maul = 466,
	WPN_Mailbox = 457,
	WPN_ThirdDegree = 593,
	WPN_Lollychop = 739,
	WPN_NeonAnnihilator1 = 813,
	WPN_NeonAnnihilator2 = 834,
	WPN_FestiveAxtingisher = 1000,
};
enum demomanweapons
{
	//Primary
	WPN_GrenadeLauncher = 19,
	WPN_NewGrenadeLauncher = 206,
	WPN_LochNLoad = 308,
	WPN_LoooseCannon = 996,
	WPN_FestiveGrenadeLauncher = 1007,
	WPN_IronBomber = 1151,
	//Secondary
	WPN_StickyLauncher = 20,
	WPN_NewStickyLauncher = 207,
	WPN_ScottishResistance = 130,
	WPN_StickyJumper = 265,
	WPN_FestiveStickyLauncher = 661,
	WPN_BotStickyS = 797,
	WPN_BotStickyG = 806,
	WPN_BotStickyR = 886,
	WPN_BotStickyB = 895,
	WPN_BotStickyC = 904,
	WPN_BotStickyD = 913,
	WPN_BotStickyES = 962,
	WPN_BotStickyEG = 971,
	WPN_QuickieBombLauncher = 1150,
	WPN_SplendidScreen = 406,
	WPN_CharginTarge = 131,
	WPN_FestiveCharginTarge = 1144,
	WPN_TideTurner = 1099,
	//Melee
	WPN_Bottle = 1,
	WPN_NewBottle = 191,
	WPN_Sword = 132,
	WPN_ScottsSkullctter = 172,
	WPN_Fryingpan = 264,
	WPN_Headless = 266,
	WPN_Ullapool = 307,
	WPN_Claidheamhmor = 327,
	WPN_PersainPersuader = 404,
	WPN_Golfclub = 482,
	WPN_ScottishHandshake = 609,
	WPN_GoldenFryingPan = 1071,
	WPN_FestiveEyelander = 1082,
};
enum heavyweapons
{
	//Primary
	WPN_Minigun = 15,
	WPN_NewMinigun = 202,
	WPN_Natascha = 41,
	WPN_IronCurtain = 298,
	WPN_BrassBeast = 312,
	WPN_Tomislav = 424,
	WPN_FestiveMinigun = 654,
	WPN_HuoLongHeatmaker1 = 811,
	WPN_HuoLongHeatmaker2 = 832,
	WPN_BotMinigunS = 793,
	WPN_BotMinigunG = 802,
	WPN_BotMinigunR = 882,
	WPN_BotMinigunB = 891,
	WPN_BotMinigunC = 900,
	WPN_BotMinigunD = 909,
	WPN_Deflector = 850,
	WPN_BotMinigunES = 958,
	WPN_BotMinigunEG = 967,
	//Secondary
	WPN_HeavyShotgun = 11,
	WPN_Sandvich = 42,
	WPN_CandyBar = 159,
	WPN_Steak = 311,
	WPN_Fishcake = 433,
	WPN_FamilyBuisness = 425,
	WPN_RobotSandvich = 863,
	WPN_FestiveSandvich = 1002,
	//Melee
	WPN_Fists = 5,
	WPN_NewFists = 195,
	WPN_KGB = 43,
	WPN_GRU = 239,
	WPN_WarriorSpirit = 310,
	WPN_FistsOfSteel = 331,
	WPN_EvictionNotice = 426,
	WPN_ApocoFists = 587,
	WPN_HolidayPunch = 656,
	WPN_FestiveGRU = 1084,
	WPN_BreadBite = 1100,
};
enum engineerweapons
{
	//Primary
	WPN_EngineerShotgun = 9,
	WPN_FrontierJustice = 141,
	WPN_Widowmaker = 527,
	WPN_Pomson = 588,
	WPN_RescueRanger = 997,
	WPN_FestiveFrontierJustice = 1004,
	//Secondary
	WPN_EngineerPistol = 22,
	WPN_Wrangler = 140,
	WPN_ShortCircut = 528,
	WPN_FestiveWrangler = 1086,
	WPN_GeigerCounter = 30668,
	//Melee
	WPN_Wrench = 7,
	WPN_NewWrench = 197,
	WPN_Goldenwrench = 169,
	WPN_SouthernHospitality = 155,
	WPN_Gunslinger = 142,
	WPN_Jag = 329,
	WPN_FestiveWrench = 662,
	WPN_EurekaEffect = 589,
	WPN_BotWrenchS = 795,
	WPN_BotWrenchG = 804,
	WPN_BotWrenchR = 884,
	WPN_BotWrenchB = 893,
	WPN_BotWrenchC = 902,
	WPN_BotWrenchD = 911,
	WPN_BotWrenchES = 960,
	WPN_BotWrenchEG = 969,
	//Misc
	WPN_Builder = 25,
	WPN_Builder2 = 737,
	WPN_Destructor = 26,
	WPN_Toolbox = 28,
};
enum medicweapons
{
	//Primary
	WPN_SyringeGun = 17,
	WPN_NewSyringeGun = 204,
	WPN_Blutsauger = 36,
	WPN_Crossbow = 305,
	WPN_Overdose = 412,
	WPN_FestiveCrossbow = 1079,
	//Secondary
	WPN_Medigun = 29,
	WPN_NewMedigun = 211,
	WPN_Kritzkrieg = 35,
	WPN_QuickFix = 411,
	WPN_FestiveMedigun = 663,
	WPN_MedigunS = 796,
	WPN_MedigunG = 805,
	WPN_MedigunR = 885,
	WPN_MedigunB = 894,
	WPN_MedigunC = 903,
	WPN_MedigunD = 912,
	WPN_Vaccinator = 998,
	WPN_MedigunES = 961,
	WPN_MedigunEG = 970,
	//Melee
	WPN_Bonesaw = 8,
	WPN_NewBonesaw = 198,
	WPN_Ubersaw = 37,
	WPN_Vitasaw = 173,
	WPN_Amputator = 304,
	WPN_Solemnvow = 413,
	WPN_FestiveUbersaw = 1003,
};
enum sniperweapons
{
	//Primary
	WPN_SniperRifle = 14,
	WPN_NewSniperRifle = 201,
	WPN_Huntsman = 56,
	WPN_SydneySleeper = 230,
	WPN_Bazaarbargain = 402,
	WPN_Machina = 526,
	WPN_FestiveSniperRifle = 664,
	WPN_HitmanHeatmaker = 752,
	WPN_BotSniperRifleS = 792,
	WPN_BotSniperRifleG = 801,
	WPN_BotSniperRifleR = 881,
	WPN_BotSniperRifleB = 890,
	WPN_BotSniperRifleC = 899,
	WPN_BotSniperRifleD = 908,
	WPN_AWP = 851,
	WPN_BotSniperRifleES = 957,
	WPN_BotSniperRifleEG = 966,
	WPN_FestiveHuntsman = 1005,
	WPN_CompoundBow = 1092,
	WPN_ClassicSniperRifle = 1098,
	WPN_ShootingStar = 30665,
	//Secondary
	WPN_SMG = 16,
	WPN_NewSMG = 203,
	WPN_Jarate = 58,
	WPN_DarwinDangerShield = 231,
	WPN_CleanersCarbine = 751,
	WPN_FestiveJarate = 1083,
	WPN_SelfAwareBeautyMark = 1105,
	WPN_RAZORBACK = 57,
	WPN_COZYCAMPER = 642,
	//Melee
	WPN_Kukri = 3,
	WPN_NewKukri = 193,
	WPN_TribalmansShiv = 171,
	WPN_Bushwacka = 232,
	WPN_Shahanshah = 401,
};
enum spyweapons
{
	//Primary
	WPN_Revolver = 24,
	WPN_NewRevolver = 210,
	WPN_Ambassador = 61,
	WPN_BigKill = 161,
	WPN_Letranger = 224,
	WPN_Enforcer = 460,
	WPN_Diamondback = 525,
	WPN_FestiveAmbassador = 1006,
	//Melee
	WPN_Knife = 4,
	WPN_NewKnife = 194,
	WPN_EternalReward = 225,
	WPN_DisguiseKit = 27,
	WPN_Kunai = 356,
	WPN_BigEarner = 461,
	WPN_WangaPrick = 574,
	WPN_SharpDresser = 638,
	WPN_Spycicle = 649,
	WPN_FestiveKnife = 665,
	WPN_BlackRose = 727,
	WPN_BotKnifeS = 794,
	WPN_BotKnifeG = 803,
	WPN_BotKnifeR = 883,
	WPN_BotKnifeB = 892,
	WPN_BotKnifeC = 901,
	WPN_BotKnifeD = 910,
	WPN_BotKnifeES = 959,
	WPN_BotKnifeEG = 968,
	//Watches
	WPN_InivisWatch = 30,
	WPN_DeadRinger = 59,
	WPN_CloakAndDagger = 60,
	WPN_TimePiece = 297,
	//Sapper
	WPN_Sapper = 735,
	WPN_Sapper2 = 736,
	WPN_RedTape1 = 810,
	WPN_RedTape2 = 831,
	WPN_ApSap = 933,
	WPN_FestiveSapper = 1080,
	WPN_SnackAttack = 1102,
};


class c_base_weapon : public IClientEntity
{
public:

	NETVAR( m_iItemDefinitionIndex, "m_iItemDefinitionIndex", "DT_BaseAttributableItem", 0, int );
	NETVAR( m_flNextPrimaryAttack, "m_flNextPrimaryAttack", "DT_TFWeaponBase", 0, float );
	NETVAR( m_nInspectStage, "m_nInspectStage", "DT_TFWeaponBase", 0, int );
	NETVAR( m_flInspectAnimTime, "m_flInspectAnimTime", "DT_TFWeaponBase", 0, float );
	NETVAR( m_AttributeList, "m_AttributeList", "DT_BaseAttributableItem", 0, c_attribute_list* )
	NETVAR( m_hOwnerEntity, "m_hOwnerEntity", "DT_BaseEntity", 0, uint32_t );
	NETVAR( m_iClip1, "m_iClip1", "DT_BaseCombatWeapon", 0, int );
	NETVAR( m_hBuilder, "m_hBuilder", "DT_BaseObject", 0, uint32_t );
	void get_weapon_class( );

	bool is_sniper( ) { 
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_SniperRifle:
		case WPN_NewSniperRifle:
		case WPN_Huntsman:
		case WPN_SydneySleeper:
		case WPN_Bazaarbargain:
		case WPN_Machina:
		case WPN_FestiveSniperRifle:
		case WPN_HitmanHeatmaker:
		case WPN_BotSniperRifleS:
		case WPN_BotSniperRifleG:
		case WPN_BotSniperRifleR:
		case WPN_BotSniperRifleB:
		case WPN_BotSniperRifleC:
		case WPN_BotSniperRifleD:
		case WPN_AWP:
		case WPN_BotSniperRifleES:
		case WPN_BotSniperRifleEG:
		case WPN_FestiveHuntsman:
		case WPN_CompoundBow:
		case WPN_ClassicSniperRifle:
		case WPN_ShootingStar:
			return true;
			break;
		default:
			return false;
		}
	}


	bool has_trajectory( ) {
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_Sandman:
		case WPN_WrapAssassin:
		case WPN_FlyingGuillotine1:
		case WPN_FlyingGuillotine2:
		case WPN_Flaregun:
		case WPN_Detonator:
		case WPN_ScorchShot:
		case WPN_FestiveFlaregun:
		case WPN_GrenadeLauncher:
		case WPN_NewGrenadeLauncher:
		case WPN_LochNLoad:
		case WPN_LoooseCannon:
		case WPN_FestiveGrenadeLauncher:
		case WPN_IronBomber:
		case WPN_Ullapool:
		case WPN_QuickieBombLauncher:
		case WPN_StickyJumper:
		case WPN_NewStickyLauncher:
		case WPN_StickyLauncher:
		case WPN_ScottishResistance:
		case WPN_FestiveHuntsman:
		case WPN_Huntsman:
		case WPN_FestiveJarate:
		case WPN_Jarate:
		case WPN_SelfAwareBeautyMark:
		case WPN_GasPasser:
		case WPN_CompoundBow:
			return true;
			break;
		default:
			return false;
		}
	}

	bool is_primary( ) {
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_Scattergun:
		case WPN_NewScattergun:
		case WPN_FAN:
		case WPN_Shortstop:
		case WPN_SodaPopper:
		case WPN_BabyFaceBlaster:
		case WPN_BotScattergunS:
		case WPN_BotScattergunG:
		case WPN_BotScattergunR:
		case WPN_BotScattergunB:
		case WPN_BotScattergunC:
		case WPN_BotScattergunD:
		case WPN_BotScattergunES:
		case WPN_BotScattergunEG:
		case WPN_FestiveFaN:
		case WPN_BackScatter:
		case WPN_RocketLauncher:
		case WPN_NewRocketLauncher:
		case WPN_DirectHit:
		case WPN_BlackBox:
		case WPN_RocketJumper:
		case WPN_LibertyLauncher:
		case WPN_CowMangler:
		case WPN_Original:
		case WPN_FestiveRocketLauncher:
		case WPN_BeggersBazooka:
		case WPN_BotRocketlauncherS:
		case WPN_BotRocketlauncherG:
		case WPN_BotRocketlauncherR:
		case WPN_BotRocketlauncherB:
		case WPN_BotRocketlauncherC:
		case WPN_BotRocketlauncherD:
		case WPN_BotRocketlauncherES:
		case WPN_BotRocketlauncherEG:
		case WPN_FestiveBlackbox:
		case WPN_Airstrike:
		case WPN_Flamethrower:
		case WPN_NewFlamethrower:
		case WPN_Backburner:
		case WPN_Degreaser:
		case WPN_FestiveFlamethrower:
		case WPN_Phlogistinator:
		case WPN_Rainblower:
		case WPN_BotFlamethrowerS:
		case WPN_BotFlamethrowerG:
		case WPN_BotFlamethrowerR:
		case WPN_BotFlamethrowerB:
		case WPN_BotFlamethrowerC:
		case WPN_BotFlamethrowerD:
		case WPN_BotFlamethrowerES:
		case WPN_BotFlamethrowerEG:
		case WPN_FestiveBackburner:
		case WPN_GrenadeLauncher:
		case WPN_NewGrenadeLauncher:
		case WPN_LochNLoad:
		case WPN_LoooseCannon:
		case WPN_FestiveGrenadeLauncher:
		case WPN_IronBomber:
		case WPN_Minigun:
		case WPN_NewMinigun:
		case WPN_Natascha:
		case WPN_IronCurtain:
		case WPN_BrassBeast:
		case WPN_Tomislav:
		case WPN_FestiveMinigun:
		case WPN_HuoLongHeatmaker1:
		case WPN_HuoLongHeatmaker2:
		case WPN_BotMinigunS:
		case WPN_BotMinigunG:
		case WPN_BotMinigunR:
		case WPN_BotMinigunB:
		case WPN_BotMinigunC:
		case WPN_BotMinigunD:
		case WPN_Deflector:
		case WPN_BotMinigunES:
		case WPN_BotMinigunEG:
		case WPN_EngineerShotgun:
		case WPN_FrontierJustice:
		case WPN_Widowmaker:
		case WPN_Pomson:
		case WPN_RescueRanger:
		case WPN_FestiveFrontierJustice:
		case WPN_SyringeGun:
		case WPN_NewSyringeGun:
		case WPN_Blutsauger:
		case WPN_Crossbow:
		case WPN_Overdose:
		case WPN_FestiveCrossbow:
		case WPN_SniperRifle:
		case WPN_NewSniperRifle:
		case WPN_Huntsman:
		case WPN_SydneySleeper:
		case WPN_Bazaarbargain:
		case WPN_Machina:
		case WPN_FestiveSniperRifle:
		case WPN_HitmanHeatmaker:
		case WPN_BotSniperRifleS:
		case WPN_BotSniperRifleG:
		case WPN_BotSniperRifleR:
		case WPN_BotSniperRifleB:
		case WPN_BotSniperRifleC:
		case WPN_BotSniperRifleD:
		case WPN_AWP:
		case WPN_BotSniperRifleES:
		case WPN_BotSniperRifleEG:
		case WPN_FestiveHuntsman:
		case WPN_CompoundBow:
		case WPN_ClassicSniperRifle:
		case WPN_ShootingStar:
		case WPN_Revolver:
		case WPN_NewRevolver:
		case WPN_Ambassador:
		case WPN_BigKill:
		case WPN_Letranger:
		case WPN_Enforcer:
		case WPN_Diamondback:
		case WPN_FestiveAmbassador:
			return true;
			break;
		default:
			return false;
		}
	}

	bool is_secondary( ) {
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_ScoutPistol:
		case WPN_NewPistol:
		case WPN_Bonk:
		case WPN_CritCola:
		case WPN_Lugermorph:
		case WPN_Milk:
		case WPN_Lugermorph2:
		case WPN_Winger:
		case WPN_PocketPistol:
		case WPN_MutatedMilk:
		case WPN_CAPPER:
		case WPN_SoldierShotgun:
		case WPN_NewShotgun:
		case WPN_BuffBanner:
		case WPN_BattalionBackup:
		case WPN_Concheror:
		case WPN_ReserveShooter:
		case WPN_RighteousBison:
		case WPN_FestiveBuffBanner:
		case WPN_PanicAttack:
		case WPN_ManTreads:
		case WPN_GunBoats:
		case WPN_BASEJumper:
		case WPN_PyroShotgun:
		case WPN_Flaregun:
		case WPN_Detonator:
		case WPN_ManMelter:
		case WPN_ScorchShot:
		case WPN_FestiveFlaregun:
		case WPN_ThermalThruster:
		case WPN_GasPasser:
		case WPN_StickyLauncher:
		case WPN_NewStickyLauncher:
		case WPN_ScottishResistance:
		case WPN_StickyJumper:
		case WPN_FestiveStickyLauncher:
		case WPN_BotStickyS:
		case WPN_BotStickyG:
		case WPN_BotStickyR:
		case WPN_BotStickyB:
		case WPN_BotStickyC:
		case WPN_BotStickyD:
		case WPN_BotStickyES:
		case WPN_BotStickyEG:
		case WPN_QuickieBombLauncher:
		case WPN_SplendidScreen:
		case WPN_CharginTarge:
		case WPN_FestiveCharginTarge:
		case WPN_TideTurner:
		case WPN_HeavyShotgun:
		case WPN_Sandvich:
		case WPN_CandyBar:
		case WPN_Steak:
		case WPN_Fishcake:
		case WPN_FamilyBuisness:
		case WPN_RobotSandvich:
		case WPN_FestiveSandvich:
		case WPN_EngineerPistol:
		case WPN_Wrangler:
		case WPN_ShortCircut:
		case WPN_FestiveWrangler:
		case WPN_GeigerCounter:
		case WPN_Medigun:
		case WPN_NewMedigun:
		case WPN_Kritzkrieg:
		case WPN_QuickFix:
		case WPN_FestiveMedigun:
		case WPN_MedigunS:
		case WPN_MedigunG:
		case WPN_MedigunR:
		case WPN_MedigunB:
		case WPN_MedigunC:
		case WPN_MedigunD:
		case WPN_Vaccinator:
		case WPN_MedigunES:
		case WPN_MedigunEG:
		case WPN_SMG:
		case WPN_NewSMG:
		case WPN_Jarate:
		case WPN_DarwinDangerShield:
		case WPN_CleanersCarbine:
		case WPN_FestiveJarate:
		case WPN_SelfAwareBeautyMark:
		case WPN_RAZORBACK:
		case WPN_COZYCAMPER:
			return true;
			break;
		default:
			return false;
		}
	}

	bool is_third( ) {
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_Bat:
		case WPN_NewBat:
		case WPN_Sandman:
		case WPN_Fish:
		case WPN_Cane:
		case WPN_BostonBasher:
		case WPN_SunStick:
		case WPN_FanOWar:
		case WPN_RuneBlade:
		case WPN_Saxxy:
		case WPN_Atomizer:
		case WPN_ConscientiousObjector:
		case WPN_UnarmedCombat:
		case WPN_WrapAssassin:
		case WPN_FestiveBat:
		case WPN_FestiveScattergun:
		case WPN_FlyingGuillotine1:
		case WPN_FlyingGuillotine2:
		case WPN_FreedomStaff:
		case WPN_BatOuttaHell:
		case WPN_MemoryMaker:
		case WPN_FestiveFish:
		case WPN_TheHamShank:
		case WPN_CrossingGuard:
		case WPN_NecroSmasher:
		case WPN_Batsaber:
		case WPN_Shovel:
		case WPN_NewShovel:
		case WPN_Equalizer:
		case WPN_PainTrain:
		case WPN_Katana:
		case WPN_MarketGardener:
		case WPN_DisciplinaryAction:
		case WPN_EscapePlan:
		case WPN_Fireaxe:
		case WPN_NewAxe:
		case WPN_Axtingusher:
		case WPN_HomeWrecker:
		case WPN_PowerJack:
		case WPN_Backscratcher:
		case WPN_VolcanoFragment:
		case WPN_Maul:
		case WPN_Mailbox:
		case WPN_ThirdDegree:
		case WPN_Lollychop:
		case WPN_NeonAnnihilator1:
		case WPN_NeonAnnihilator2:
		case WPN_FestiveAxtingisher:
		case WPN_Bottle:
		case WPN_NewBottle:
		case WPN_Sword:
		case WPN_ScottsSkullctter:
		case WPN_Fryingpan:
		case WPN_Headless:
		case WPN_Ullapool:
		case WPN_Claidheamhmor:
		case WPN_PersainPersuader:
		case WPN_Golfclub:
		case WPN_ScottishHandshake:
		case WPN_GoldenFryingPan:
		case WPN_FestiveEyelander:
		case WPN_Fists:
		case WPN_NewFists:
		case WPN_KGB:
		case WPN_GRU:
		case WPN_WarriorSpirit:
		case WPN_FistsOfSteel:
		case WPN_EvictionNotice:
		case WPN_ApocoFists:
		case WPN_HolidayPunch:
		case WPN_FestiveGRU:
		case WPN_BreadBite:
		case WPN_Wrench:
		case WPN_NewWrench:
		case WPN_Goldenwrench:
		case WPN_SouthernHospitality:
		case WPN_Gunslinger:
		case WPN_Jag:
		case WPN_FestiveWrench:
		case WPN_EurekaEffect:
		case WPN_BotWrenchS:
		case WPN_BotWrenchG:
		case WPN_BotWrenchR:
		case WPN_BotWrenchB:
		case WPN_BotWrenchC:
		case WPN_BotWrenchD:
		case WPN_BotWrenchES:
		case WPN_BotWrenchEG:
		case WPN_Bonesaw:
		case WPN_NewBonesaw:
		case WPN_Ubersaw:
		case WPN_Vitasaw:
		case WPN_Amputator:
		case WPN_Solemnvow:
		case WPN_FestiveUbersaw:
		case WPN_Kukri:
		case WPN_NewKukri:
		case WPN_TribalmansShiv:
		case WPN_Bushwacka:
		case WPN_Shahanshah:
			return true;
			break;
		default:
			return false;
		}
	}

	bool is_shield( ) {
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_CharginTarge:
		case WPN_SplendidScreen:
		case WPN_TideTurner:
		case WPN_FestiveCharginTarge:
			return true;
			break;
		default:
			return false;
		}
	}


	bool is_shotgun( ) { 
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_Scattergun:
		case WPN_NewScattergun:
		case WPN_FAN:
		case WPN_SodaPopper:
		case WPN_FestiveFaN:
		case WPN_BackScatter:
		case WPN_SoldierShotgun:
		case WPN_NewShotgun:
		case WPN_ReserveShooter:
		case WPN_PanicAttack:
		case WPN_PyroShotgun:
		case WPN_HeavyShotgun:
		case WPN_FamilyBuisness:
		case WPN_EngineerShotgun:
		case WPN_FrontierJustice:
		case WPN_Widowmaker:
		case WPN_Pomson:
		case WPN_RescueRanger:
		case WPN_FestiveFrontierJustice:
			return true;
			break;
		default:
			return false;
		}
	}

	bool is_melee( ) { 
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_Bat:
		case WPN_NewBat:
		case WPN_Sandman:
		case WPN_Fish:
		case WPN_Cane:
		case WPN_BostonBasher:
		case WPN_SunStick:
		case WPN_FanOWar:
		case WPN_RuneBlade:
		case WPN_Saxxy:
		case WPN_Atomizer:
		case WPN_ConscientiousObjector:
		case WPN_UnarmedCombat:
		case WPN_WrapAssassin:
		case WPN_FestiveBat:
		case WPN_FreedomStaff:
		case WPN_BatOuttaHell:
		case WPN_MemoryMaker:
		case WPN_FestiveFish:
		case WPN_TheHamShank:
		case WPN_CrossingGuard:
		case WPN_NecroSmasher:
		case WPN_Batsaber:
		case WPN_Shovel:
		case WPN_NewShovel:
		case WPN_Equalizer:
		case WPN_PainTrain:
		case WPN_Katana:
		case WPN_MarketGardener:
		case WPN_DisciplinaryAction:
		case WPN_EscapePlan:
		case WPN_Fireaxe:
		case WPN_NewAxe:
		case WPN_Axtingusher:
		case WPN_HomeWrecker:
		case WPN_PowerJack:
		case WPN_Backscratcher:
		case WPN_VolcanoFragment:
		case WPN_Maul:
		case WPN_Mailbox:
		case WPN_ThirdDegree:
		case WPN_Lollychop:
		case WPN_NeonAnnihilator1:
		case WPN_NeonAnnihilator2:
		case WPN_FestiveAxtingisher:
		case WPN_Bottle:
		case WPN_NewBottle:
		case WPN_Sword:
		case WPN_ScottsSkullctter:
		case WPN_Fryingpan:
		case WPN_Headless:
		case WPN_Ullapool:
		case WPN_Claidheamhmor:
		case WPN_PersainPersuader:
		case WPN_Golfclub:
		case WPN_ScottishHandshake:
		case WPN_GoldenFryingPan:
		case WPN_FestiveEyelander:
		case WPN_Fists:
		case WPN_NewFists:
		case WPN_KGB:
		case WPN_GRU:
		case WPN_WarriorSpirit:
		case WPN_FistsOfSteel:
		case WPN_EvictionNotice:
		case WPN_ApocoFists:
		case WPN_HolidayPunch:
		case WPN_FestiveGRU:
		case WPN_BreadBite:
		case WPN_Wrench:
		case WPN_NewWrench:
		case WPN_Goldenwrench:
		case WPN_SouthernHospitality:
		case WPN_Gunslinger:
		case WPN_Jag:
		case WPN_Bonesaw:
		case WPN_NewBonesaw:
		case WPN_Ubersaw:
		case WPN_Vitasaw:
		case WPN_Amputator:
		case WPN_Solemnvow:
		case WPN_FestiveUbersaw:
		case WPN_Kukri:
		case WPN_NewKukri:
		case WPN_TribalmansShiv:
		case WPN_Bushwacka:
		case WPN_Shahanshah:
			return true;
			break;
		default:
			return false;
		}
	}
	
	bool is_pistol( ) { 
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_EngineerPistol:
		case WPN_ScoutPistol:
		case WPN_NewPistol:
		case WPN_Lugermorph:
		case WPN_Lugermorph2:
		case WPN_Winger:
		case WPN_PocketPistol:
		case WPN_CAPPER:
		case WPN_ShortCircut:
		case WPN_Flaregun:
		case WPN_FestiveFlaregun:
		case WPN_Detonator:
		case WPN_ManMelter:
		case WPN_ScorchShot:
		case WPN_Shortstop:
			return true;
			break;
		default:
			return false;
		}
	}
	
	bool is_grenade( ) { 
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_Milk:
		case WPN_MutatedMilk:
		case WPN_Jarate:
		case WPN_FestiveJarate:
		case WPN_SelfAwareBeautyMark:
		case WPN_GasPasser:
		case WPN_FlyingGuillotine1:
		case WPN_FlyingGuillotine2:
			return true;
			break;
		default:
			return false;
		
		}
	}
	

	bool is_util( ) { 
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_Milk:
		case WPN_MutatedMilk:
		case WPN_Jarate:
		case WPN_FestiveJarate:
		case WPN_DarwinDangerShield:
		case WPN_RAZORBACK:
		case WPN_COZYCAMPER:
		case WPN_BuffBanner:
		case WPN_BattalionBackup:
		case WPN_Concheror:
		case WPN_BASEJumper:
		case WPN_ManTreads:
		case WPN_GunBoats:
		case WPN_ThermalThruster:
		case WPN_DisguiseKit:
		case WPN_Builder:
		case WPN_Builder2:
		case WPN_Destructor:
		case WPN_Toolbox:
			return true;
			break;
		default:
			return false;

		}
	}
	
	bool is_flame_thrower( ) { 
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_Flamethrower:
		case WPN_NewFlamethrower:
		case WPN_Backburner:
		case WPN_Degreaser:
		case WPN_FestiveFlamethrower:
		case WPN_Phlogistinator:
		case WPN_Rainblower:
		case WPN_BotFlamethrowerS:
		case WPN_BotFlamethrowerG:
		case WPN_BotFlamethrowerR:
		case WPN_BotFlamethrowerB:
		case WPN_BotFlamethrowerC:
		case WPN_BotFlamethrowerD:
		case WPN_BotFlamethrowerES:
		case WPN_BotFlamethrowerEG:
		case WPN_FestiveBackburner:
			return true;
			break;
		default:
			return false;
		}
	}
	
	bool is_sticky( ) {
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_StickyLauncher:
		case WPN_NewStickyLauncher:
		case WPN_ScottishResistance:
		case WPN_StickyJumper:
		case WPN_FestiveStickyLauncher:
		case WPN_BotStickyS:
		case WPN_BotStickyG:
		case WPN_BotStickyR:
		case WPN_BotStickyB:
		case WPN_BotStickyC:
		case WPN_BotStickyD:
		case WPN_BotStickyES:
		case WPN_BotStickyEG:
		case WPN_QuickieBombLauncher:
			return true;
			break;
		default:
			return false;
		}
	}

	bool is_grenade_launcher( ) { 
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_GrenadeLauncher:
		case WPN_NewGrenadeLauncher:
		case WPN_LochNLoad:
		case WPN_LoooseCannon:
		case WPN_FestiveGrenadeLauncher:
		case WPN_IronBomber:
			return true;
			break;
		default:
			return false;
		}
	}
	
	bool is_rocket_launcher( ) { 
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_RocketLauncher:
		case WPN_NewRocketLauncher:
		case WPN_DirectHit:
		case WPN_BlackBox:
		case WPN_RocketJumper:
		case WPN_LibertyLauncher:
		case WPN_CowMangler:
		case WPN_Original:
		case WPN_FestiveRocketLauncher:
		case WPN_BeggersBazooka:
		case WPN_BotRocketlauncherS:
		case WPN_BotRocketlauncherG:
		case WPN_BotRocketlauncherR:
		case WPN_BotRocketlauncherB:
		case WPN_BotRocketlauncherC:
		case WPN_BotRocketlauncherD:
		case WPN_BotRocketlauncherES:
		case WPN_BotRocketlauncherEG:
		case WPN_FestiveBlackbox:
		case WPN_Airstrike:
			return true;
			break;
		default:
			return false;
		}
	}
	
	bool is_minigun( ) {
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_Minigun:
		case WPN_NewMinigun:
		case WPN_Natascha:
		case WPN_IronCurtain:
		case WPN_BrassBeast:
		case WPN_Tomislav:
		case WPN_FestiveMinigun:
		case WPN_HuoLongHeatmaker1:
		case WPN_HuoLongHeatmaker2:
		case WPN_BotMinigunS:
		case WPN_BotMinigunG:
		case WPN_BotMinigunR:
		case WPN_BotMinigunB:
		case WPN_BotMinigunC:
		case WPN_BotMinigunD:
		case WPN_Deflector:
		case WPN_BotMinigunES:
		case WPN_BotMinigunEG:
			return true;
			break;
		default:
			return false;
		}
	}
	
	bool is_edible( ) {
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_Bonk:
		case WPN_CritCola:
		case WPN_CandyBar:
		case WPN_Steak:
		case WPN_Fishcake:
		case WPN_Sandvich:
		case WPN_RobotSandvich:
		case WPN_FestiveSandvich:
			return true;
			break;
		default:
			return false;
		}
	}
	
	bool is_wrench( ) {
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_Wrench:
		case WPN_NewWrench:
		case WPN_Goldenwrench:
		case WPN_SouthernHospitality:
		case WPN_Gunslinger:
		case WPN_Jag:
		case WPN_FestiveWrench:
		case WPN_EurekaEffect:
		case WPN_BotWrenchS:
		case WPN_BotWrenchG:
		case WPN_BotWrenchR:
		case WPN_BotWrenchB:
		case WPN_BotWrenchC:
		case WPN_BotWrenchD:
		case WPN_BotWrenchES:
		case WPN_BotWrenchEG:
			return true;
			break;
		default:
			return false;
		}
	}
	
	bool is_engineer( ) {
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_Builder:
		case WPN_Builder2:
		case WPN_Destructor:
		case WPN_Toolbox:
			return true;
			break;
		default:
			return false;
		}
	}

	bool is_needle_gun( ) {
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_SyringeGun:
		case WPN_NewSyringeGun:
		case WPN_Blutsauger:
		case WPN_Crossbow:
		case WPN_Overdose:
		case WPN_FestiveCrossbow:
			return true;
			break;
		default:
			return false;
		}
	}
	
	bool is_healing( ) { 
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_Medigun:
		case WPN_NewMedigun:
		case WPN_Kritzkrieg:
		case WPN_QuickFix:
		case WPN_FestiveMedigun:
		case WPN_MedigunS:
		case WPN_MedigunG:
		case WPN_MedigunR:
		case WPN_MedigunB:
		case WPN_MedigunC:
		case WPN_MedigunD:
		case WPN_Vaccinator:
		case WPN_MedigunES:
		case WPN_MedigunEG:
			return true;
			break;
		default:
			return false;
		}
	}
	
	bool is_revolver( ) { 
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_Revolver:
		case WPN_NewRevolver:
		case WPN_Ambassador:
		case WPN_BigKill:
		case WPN_Letranger:
		case WPN_Enforcer:
		case WPN_Diamondback:
		case WPN_FestiveAmbassador:
			return true;
			break;
		default:
			return false;
		}
	}
	
	bool is_spy_knife( ) { 
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_Knife:
		case WPN_NewKnife:
		case WPN_EternalReward:
		case WPN_DisguiseKit:
		case WPN_Kunai:
		case WPN_BigEarner:
		case WPN_WangaPrick:
		case WPN_SharpDresser:
		case WPN_Spycicle:
		case WPN_FestiveKnife:
		case WPN_BlackRose:
		case WPN_BotKnifeS:
		case WPN_BotKnifeG:
		case WPN_BotKnifeR:
		case WPN_BotKnifeB:
		case WPN_BotKnifeC:
		case WPN_BotKnifeD:
		case WPN_BotKnifeES:
		case WPN_BotKnifeEG:
			return true;
			break;
		default:
			return false;
		}
	}
	
	bool is_watch( ) { 
		switch( m_iItemDefinitionIndex( ) ) { 
		case WPN_InivisWatch:
		case WPN_DeadRinger:
		case WPN_CloakAndDagger:
		case WPN_TimePiece:
			return true;
			break;
		default:
			return false;
		}
	}
	
	bool is_sapper( ) { 
		switch( m_iItemDefinitionIndex( ) ) { 
		case WPN_Sapper:
		case WPN_Sapper2:
		case WPN_RedTape1:
		case WPN_RedTape2:
		case WPN_ApSap:
		case WPN_FestiveSapper:
		case WPN_SnackAttack:
			return true;
			break;
		default:
			return false;
		}
	}

	/*bool get_projectile_speed( float& speed ) {
		speed = 0;
		switch( m_iItemDefinitionIndex( ) ) {
		case WPN_Milk:
		case WPN_MutatedMilk:
			speed = 1019.9;
			break;
		case WPN_FlyingGuillotine1:
		case WPN_FlyingGuillotine2:
		case WPN_Sandman:
		case WPN_WrapAssassin:
		case WPN_ManMelter:
			speed = 3000;
			break;
		case WPN_RocketJumper:
		case WPN_BlackBox:
		case WPN_Original:
		case WPN_BeggersBazooka:
		case WPN_CowMangler:
			speed = 1100;
			break;

		case WPN_DirectHit:
			speed = 1980;
			break;

		case WPN_LibertyLauncher:
			speed = 1540;
			break;

		case WPN_RighteousBison:
		case WPN_Pomson:
			speed = 1200;
			break;

		case WPN_Flaregun:
		case WPN_FestiveFlaregun:
		case WPN_ScorchShot:
		case WPN_Detonator:
			speed = 2000;
			break;

		case WPN_GrenadeLauncher:
		case WPN_NewGrenadeLauncher:
		case WPN_FestiveGrenadeLauncher:
		case WPN_IronBomber:
			speed = 1216.6;
			break;



		/*
		get dynamically
		
		stickybomb launcher
		scottish resistance
		sticky jumper
		quickiebomb launcher
		huntman fortified compound

		

		}
		return ( speed );
	}*/

	int get_max_clip_1( );
	int get_max_clip_2( );
	int get_slot( );
	char* get_name( );
	char* get_print_name( );
	vec3_t& get_bullet_spread( );
	float get_spread( );
	int get_weaponid( );
	bool can_fire( );
	bool is_non_aim( );
	int& get_inspect_stage( );
	float& get_inspect_time( );
	c_attribute_list* get_attribute_list( );
	float get_distance( );
};

#ifndef __INC_METIN_II_GAME_CONFIG_H__
#define __INC_METIN_II_GAME_CONFIG_H__

enum
{
	ADDRESS_MAX_LEN = 15
};

void config_init(const std::string& st_localeServiceName); // default "" is CONFIG

extern char sql_addr[256];

extern WORD mother_port;
extern WORD p2p_port;

extern char db_addr[ADDRESS_MAX_LEN + 1];
extern WORD db_port;

extern char teen_addr[ADDRESS_MAX_LEN + 1];
extern WORD teen_port;

extern char passpod_addr[ADDRESS_MAX_LEN + 1];
extern WORD passpod_port;

extern int passes_per_sec;
extern int save_event_second_cycle;
extern int ping_event_second_cycle;
extern int test_server;
extern bool	guild_mark_server;
extern BYTE guild_mark_min_level;
extern bool	distribution_test_server;
extern bool	china_event_server;

extern bool	g_bNoMoreClient;
extern bool	g_bNoRegen;

extern bool	g_bTrafficProfileOn;		///< true 이면 TrafficProfiler 를 켠다.

extern BYTE	g_bChannel;

extern bool	map_allow_find(int index);
extern void	map_allow_copy(long * pl, int size);
extern bool	no_wander;

extern int	g_iUserLimit;
extern time_t	g_global_time;

const char *	get_table_postfix();

extern std::string	g_stHostname;
extern std::string	g_stLocale;
extern std::string	g_stLocaleFilename;

extern char		g_szPublicIP[16];
extern char		g_szInternalIP[16];

extern int (*is_twobyte) (const char * str);
extern int (*check_name) (const char * str);

extern bool		g_bSkillDisable;

extern int		g_iFullUserCount;
extern int		g_iBusyUserCount;
extern void		LoadStateUserCount();

extern bool	g_bEmpireWhisper;

extern BYTE	g_bAuthServer;
extern BYTE	g_bBilling;

extern BYTE	PK_PROTECT_LEVEL;

extern void	LoadValidCRCList();
extern bool	IsValidProcessCRC(DWORD dwCRC);
extern bool	IsValidFileCRC(DWORD dwCRC);

extern std::string	g_stAuthMasterIP;
extern WORD		g_wAuthMasterPort;

extern std::string	gServerVersion;
extern std::string	g_stClientVersion;

extern bool		g_bCheckClientVersion;
extern void		CheckClientVersion();

extern std::string	g_stQuestDir;
//extern std::string	g_stQuestObjectDir;
extern std::set<std::string> g_setQuestObjectDir;


extern std::vector<std::string>	g_stAdminPageIP;
extern std::string	g_stAdminPagePassword;

extern int	SPEEDHACK_LIMIT_COUNT;
extern int 	SPEEDHACK_LIMIT_BONUS;

extern int g_iSyncHackLimitCount;

extern int g_server_id;
extern std::string g_strWebMallURL;

extern int VIEW_RANGE;
extern int VIEW_BONUS_RANGE;

extern bool g_bCheckMultiHack;
extern bool g_protectNormalPlayer;      // 범법자가 "평화모드" 인 일반유저를 공격하지 못함
extern bool g_noticeBattleZone;         // 중립지대에 입장하면 안내메세지를 알려줌

extern DWORD g_GoldDropTimeLimitValue;

extern bool isHackShieldEnable;
extern int  HackShield_FirstCheckWaitTime;
extern int  HackShield_CheckCycleTime;
extern bool bXTrapEnabled;

extern int gPlayerMaxLevel;
extern int gPlayerMaxLevelStats;
extern int gPlayerMaxStatus;
extern int gPLayerMaxHT;
extern int gPLayerMaxIQ;
extern int gPLayerMaxST;
extern int gPLayerMaxDX;

extern int minAddonFKS;
extern int maxAddonFKS;
extern int minAddonDSS;
extern int maxAddonDSS;

extern int attr_change_limit;
extern bool attr_always_add;
extern bool attr_always_5_add;
extern bool stone_always_add;
extern int guild_max_level;
extern int item_floor_time;
extern int item_ownership_time;
extern int item_drop_time;
extern int gold_drop_time;

extern int sb_delay;
extern int sb_need_exp;
extern bool sb_always_book;

extern bool sequenche_check;
extern bool syserr_enable;
extern bool syslog_enable;
extern bool glass_enable;
extern bool glass_needed;
extern bool skillbook_step_leveling;
extern int max_skills;
extern int horse_max_level;
extern int marriage_max_percent;
extern int pc_max_movement_speed;
extern int pc_max_attack_speed;
extern int mob_max_movement_speed;
extern int mob_max_attack_speed;
extern int dye_level;
extern int taxes;
extern bool raise_empire_prices;
extern long long yang_max;
extern bool package_enable;
extern DWORD g_ItemDropTimeLimitValue;
extern bool g_stAdminPageEnable;
extern bool g_stAdminPageNoLChost;
extern bool new_test_server;
extern bool new_gm_host_check;
extern int log_level;
extern bool belt_allow_all_items;
extern int belt_force_slots;
extern long max_rank_points;
extern int movement_speed;
extern bool attr_rare_enable;
extern bool global_chat;
extern bool bigger_inventory;
extern bool ban_force_reason;
extern bool trade_effect;
extern bool advanced_spam_check;
extern bool view_equip_for_all;
extern int view_equip_sp_cost;
extern long int start_gold;
extern int trade_effect_exchange_threshold;
extern int trade_effect_shop_threshold;
extern int versioncheck_kick_delay;
extern bool bugfix_sura_manashield;
extern bool emotion_without_mask;
extern bool emotion_same_gender;
extern bool check_speedhack_enable;
extern int skill_master_upgrade;
extern bool skill_force_master;
extern long long EXP_NEED_THRESHOLD;

extern long long LLMINMAX(long long min, long long value, long long max);

extern bool g_BlockCharCreation;
extern int shopRespawn;


/// Herra CONFIG ///
extern std::string serveur;
extern int index_map1_jaune;
extern int index_map1_bleu;
extern int index_map1_rouge;
extern int position_map1_jaune_x;
extern int position_map1_jaune_y;
extern int position_map1_bleu_x;
extern int position_map1_bleu_y;
extern int position_map1_rouge_x;
extern int position_map1_rouge_y;
extern int pierre_instant_ajout;
extern int pierre_pourcent_ajout;
extern int degat_protection_sura;

extern int SYSTEM_CHOIX_VNUM_SHOP;
extern int SYSTEM_CARTE_CADEAU;
extern int SYSTEM_NOUVEAU_SKILL_7ET8;
extern int SYSTEM_BUFF_GROUPE;
extern int SYSTEM_TITRE;
extern int SYSTEM_OFFLINE_SHOP;
extern int SYSTEM_RATE;
extern int RATE_DROP_ITEM;
extern int RATE_DROP_GOLD;
extern int RATE_DROP_EXP;
extern int rateActive;
extern bool MobScaleActive;
extern int DESACTIVE_DROP_MANUEL;
#endif /* __INC_METIN_II_GAME_CONFIG_H__ */


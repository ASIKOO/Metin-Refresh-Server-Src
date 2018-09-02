#include "stdafx.h"
#include "char.h"

#include "config.h"
#include "utils.h"
#include "crc32.h"
#include "char_manager.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "buffer_manager.h"
#include "item_manager.h"
#include "motion.h"
#include "vector.h"
#include "packet.h"
#include "cmd.h"
#include "fishing.h"
#include "exchange.h"
#include "battle.h"
#include "affect.h"
#include "shop.h"
#include "shop_manager.h"
#include "safebox.h"
#include "regen.h"
#include "pvp.h"
#include "party.h"
#include "start_position.h"
#include "questmanager.h"
#include "log.h"
#include "p2p.h"
#include "guild.h"
#include "guild_manager.h"
#include "dungeon.h"
#include "messenger_manager.h"
#include "unique_item.h"
#include "priv_manager.h"
#include "war_map.h"
#include "xmas_event.h"
#include "banword.h"
#include "target.h"
#include "wedding.h"
#include "mob_manager.h"
#include "mining.h"
#include "monarch.h"
#include "castle.h"
#include "arena.h"
#include "dev_log.h"
#include "horsename_manager.h"
#include "pcbang.h"
#include "gm.h"
#include "map_location.h"
#include "BlueDragon_Binder.h"
#include "HackShield.h"
#include "skill_power.h"
#include "XTrapManager.h"
#include "buff_on_attributes.h"
#include "char_mob.h"
#include "db.h"
#include "entity.h"
#include <sstream>
#include <iostream>
#include <string>

using namespace std;
std::vector<int> mob_id_vector;
std::vector<std::string> mob_scale_vector;

// Met en mémoire le scale des mobs avec leurs vnums au démarrage du core.[Cela évite l'utilisation de mysql à chaque fois]
bool ChargementListeMobScaleMYSQL()
{
	if (mob_scale_vector.size() > 0)
	{
		mob_scale_vector.clear();
		mob_id_vector.clear();
	}
	fprintf(stdout, "Chargement de la table de mob_scale!\n");
	std::unique_ptr<SQLMsg> pMsgDESC(DBManager::instance().DirectQuery("SELECT vnum, mob_scale FROM mob_proto%s", get_table_postfix()));
	mob_scale_vector.reserve(pMsgDESC->Get()->uiNumRows);
	mob_id_vector.reserve(pMsgDESC->Get()->uiNumRows);

	MYSQL_ROW rowMob;
	int tmpVnum = 0;
	while((rowMob = mysql_fetch_row(pMsgDESC->Get()->pSQLResult)))
	{
		str_to_number(tmpVnum, rowMob[0]);
		mob_id_vector.emplace_back(tmpVnum);
		mob_scale_vector.emplace_back(rowMob[1]);
		tmpVnum = 0;
	}
	return true;
}

// Envoi la liste des scales Mobs/NPC/Pet au joueur lorsqu'il ce connecte.[La on n'utilise que la liste en mémoire]
void SendMobScalePaquet(LPCHARACTER ch)
{

	std::string paquetEnable = "MobScaleEnable 0";
	ch->ChatPacket(CHAT_TYPE_COMMAND, "%s", paquetEnable.c_str());

	int taille = 0;
	for (int k = 0; k < mob_id_vector.size(); k++)
	{
		taille++;
	}
	std::string paquetTaille = "MobScaleTaille ";
	std::ostringstream oss;
	oss << taille;
	paquetTaille += oss.str();
	oss.clear();
	ch->ChatPacket(CHAT_TYPE_COMMAND, "%s", paquetTaille.c_str());
	std::string paquetScale = "MobScaleValeur ";
	for (int i = 0; i < mob_scale_vector.size(); i++)
	{
		std::ostringstream oss2;
		paquetScale = "MobScaleValeur ";
		oss2 << mob_id_vector[i];
		oss2 << "|";
		oss2 << mob_scale_vector[i];
		paquetScale += oss2.str();
		ch->ChatPacket(CHAT_TYPE_COMMAND, "%s", paquetScale.c_str());
		oss2.clear();
	}
	paquetEnable = "MobScaleEnable 1";
	ch->ChatPacket(CHAT_TYPE_COMMAND, "%s", paquetEnable.c_str());
}


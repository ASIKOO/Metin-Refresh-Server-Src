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
#include "char_item_bloquer.h"
#include "db.h"
#include "entity.h"
#include <sstream>
#include <iostream>
#include <string>

using namespace std;
std::vector<int> item_bloquer_vnum_vector;
std::vector<int> map_id_vector;
std::vector<int> type1_item_vector;
std::vector<int> type2_item_vector;
std::vector<int> type3_item_vector;
std::vector<int> type4_item_vector;

// Evènement de mise à jour automatique du système de météo.
EVENTINFO(itembloquer_event_info) 
{
	LPCHARACTER ch;
};

// Fonction de mise à jour automatique du système de météo
EVENTFUNC(itembloquer_update_event) 
{
	itembloquer_event_info* info = dynamic_cast<itembloquer_event_info*>( event->info );

	LPCHARACTER ch = info->ch;
	if ( info == NULL )
	{
		sys_err( "check_speedhack_event> <Factor> Null pointer" );
		return 0;
	}
	if (ch == NULL)
	{
		return 0;
	}

	LPITEM item;
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		if ((item = ch->GetInventoryItem(i)))
		{
			if (item != NULL)
			{
				EAffectTypes type = AFFECT_NONE;

				switch (item->GetVnum())
				{
				case 72723:
				case 72724:
				case 72725:
				case 72726:
					type = AFFECT_AUTO_HP_RECOVERY;
				break;
				}

				CAffect* pAffect = ch->FindAffect( type );


				if (UtilisationObjet(ch, item, false) == false && pAffect != NULL)
				{
					if (item->GetID() == pAffect->dwFlag)
					{
						ch->RemoveAffect( pAffect );

						item->Lock(false);
						item->SetSocket(0, false);
					}
					else
					{
						LPITEM old = ch->FindItemByID( pAffect->dwFlag );

						if (NULL != old)
						{
							old->Lock(false);
							old->SetSocket(0, false);
						}

						ch->RemoveAffect( pAffect );
					}
				}
			}
		}
	}
 	return PASSES_PER_SEC(1);
}


void ItemBloquer::StartCheckItem(LPCHARACTER ch)
{
	if (ch != NULL)
	{
		if (m_pkCheckItem == NULL)
		{
			itembloquer_event_info* info = AllocEventInfo<itembloquer_event_info>();
			info->ch = ch;
			m_pkCheckItem = event_create(itembloquer_update_event, info, PASSES_PER_SEC(1));
		}
	}
}

bool LectureListeBlocageObjet()
{
	if (item_bloquer_vnum_vector.size() > 0)
	{
		item_bloquer_vnum_vector.clear();
		map_id_vector.clear();
		type1_item_vector.clear();
		type2_item_vector.clear();
		type3_item_vector.clear();
		type4_item_vector.clear();

	}
	fprintf(stdout, "Chargement de la table de blocage d'objet en cours!\n");
	std::unique_ptr<SQLMsg> pMsgDESC(DBManager::instance().DirectQuery("SELECT vnum, map, type1, type2, type3, type4 FROM player_item_bloquer%s", get_table_postfix()));
	if (pMsgDESC->Get()->uiNumRows <= 0)
	{
		return false;
	}
	item_bloquer_vnum_vector.reserve(pMsgDESC->Get()->uiNumRows);
	map_id_vector.reserve(pMsgDESC->Get()->uiNumRows);
	type1_item_vector.reserve(pMsgDESC->Get()->uiNumRows);
	type2_item_vector.reserve(pMsgDESC->Get()->uiNumRows);
	type3_item_vector.reserve(pMsgDESC->Get()->uiNumRows);
	type4_item_vector.reserve(pMsgDESC->Get()->uiNumRows);

	MYSQL_ROW rowItem;
	int tmpNombre = 0;
	while((rowItem = mysql_fetch_row(pMsgDESC->Get()->pSQLResult)))
	{
		str_to_number(tmpNombre, rowItem[0]);
		item_bloquer_vnum_vector.push_back(tmpNombre);
		str_to_number(tmpNombre, rowItem[1]);
		map_id_vector.push_back(tmpNombre);

		str_to_number(tmpNombre, rowItem[2]);
		type1_item_vector.push_back(tmpNombre);
		str_to_number(tmpNombre, rowItem[3]);
		type2_item_vector.push_back(tmpNombre);
		str_to_number(tmpNombre, rowItem[4]);
		type3_item_vector.push_back(tmpNombre);
		str_to_number(tmpNombre, rowItem[5]);
		type4_item_vector.push_back(tmpNombre);
	}
	fprintf(stdout, "Chargement de la table de blocage d'objet terminé!\n");
	return true;	
}

bool UtilisationObjet(LPCHARACTER ch, LPITEM item, bool message)
{
	if (!ch)
	{
		return false;
	}
	if (!item)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "L'objet est inexistant ou invalide.");
	}
	int VnumItem = item->GetVnum();
	for (int i = 0; i < item_bloquer_vnum_vector.size(); i++)
	{
		if (VnumItem == item_bloquer_vnum_vector[i])
		{
			if(ch->GetMapIndex() == map_id_vector[i])
			{
				if (message)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Cet objet n'est pas utilisable sur cette map.");
				}
				return false;
			}
			if(CPVPManager::Instance().HasDuel(ch) && type1_item_vector[i] == 1)
			{
				if (message)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Cet objet n'est pas utilisable en duel.");
				}
				return false;
			}
			if(ch->GetDungeon() && type2_item_vector[i] == 1)
			{
				if (message)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Cet objet n'est pas utilisable dans un donjon.");
				}
				return false;
			}
			if (ch->GetGuild() && ch->GetGuild()->IsWar == true && type3_item_vector[i] == 1)
			{
				if (message)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Cet objet n'est pas utilisable dans une guerre de guildes.");
				}
				return false;
			}
			if (type4_item_vector[i] == 1)
			{
				if (message)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Cet objet est interdit d'utilisation.");
				}
				return false;
			}
		}
	}
	return true;
}


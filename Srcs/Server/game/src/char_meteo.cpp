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
#include "char_meteo.h"
#include "db.h"
#include <sstream>
#include <iostream>
#include <string>
#include <time.h>

using namespace std;

std::vector<std::string> type_meteo_vector; // Liste des météo existante.
std::vector<std::string> heure_meteo_vector; // Liste des heures météo existante.
std::vector<std::string> environnement_meteo_vector; // Liste des environnements météo existant.
std::vector<std::string> commande_meteo_vector; // Liste des commandes météo existantes.
std::vector<std::string> musique_meteo_vector; // Liste des musiques météo existantes.
std::string environnementActuel; // Indique l'environnement actuel.
std::string commandeActuel; // Indique la commande environnementale actuel.
std::string musiqueActuel; // Indique la musique environnementale actuel.

// Evènement de mise à jour automatique du système de météo.
EVENTINFO(meteosysteme_event_info) 
{
	LPCHARACTER ch;
};

// Fonction de mise à jour automatique du système de météo
EVENTFUNC(meteosysteme_update_event) 
{
	meteosysteme_event_info* info = dynamic_cast<meteosysteme_event_info*>( event->info );

	LPCHARACTER ch = info->ch;
	if ( info == NULL )
	{
		sys_err( "check_speedhack_event> <Factor> Null pointer" );
		return 0;
	}
	if (ch == NULL || ch->m_meteoSystem == NULL)
	{
		return 0;
	}
	ch->m_meteoSystem->UpdateMeteo(ch); // Lance la mise à jour automatique.
 	return PASSES_PER_SEC(1);
}

// Charge la liste de météo.
bool ChargementMeteo()
{
	if (type_meteo_vector.size() > 0)
	{
		type_meteo_vector.clear();
		heure_meteo_vector.clear();
		environnement_meteo_vector.clear();
		commande_meteo_vector.clear();
		musique_meteo_vector.clear();
	}
	std::unique_ptr<SQLMsg> pMsgMETEO(DBManager::instance().DirectQuery("SELECT type, heure, environnement, commande, musique FROM liste_meteo%s", get_table_postfix()));
	if (pMsgMETEO->uiSQLErrno != 0)
	{
		sys_err("Impossible de charger la liste des informations de météo.");
		fprintf(stdout, "Impossible de charger la table de la météo depuis mysql!\n");
		return false;
	}
	type_meteo_vector.reserve(pMsgMETEO->Get()->uiNumRows);
	heure_meteo_vector.reserve(pMsgMETEO->Get()->uiNumRows);
	environnement_meteo_vector.reserve(pMsgMETEO->Get()->uiNumRows);
	commande_meteo_vector.reserve(pMsgMETEO->Get()->uiNumRows);
	musique_meteo_vector.reserve(pMsgMETEO->Get()->uiNumRows);

	MYSQL_ROW rowMETEO;
	while((rowMETEO = mysql_fetch_row(pMsgMETEO->Get()->pSQLResult)))
	{
		type_meteo_vector.emplace_back(rowMETEO[0]);
		heure_meteo_vector.emplace_back(rowMETEO[1]);
		environnement_meteo_vector.emplace_back(rowMETEO[2]);
		commande_meteo_vector.emplace_back(rowMETEO[3]);
		musique_meteo_vector.emplace_back(rowMETEO[4]);
	}
	return true;
}


void Meteo::UpdateMeteo(LPCHARACTER ch)
{
	if (type_meteo_vector.size() > 0)
	{

		if (ch != NULL)
		{
			if (ch->GetDungeon() == false)
			{
				if(ch->m_meteoSystem != NULL)
				{
					ch->m_meteoSystem->SelectionMeteo(ch);
					if (ch->m_meteoSystem->m_pkMeteoSystemeUpdateEvent == NULL)
					{
							ch->m_meteoSystem->LoginMeteo(ch);
					}
					if(ch->m_meteoSystem->MSJMeteo == false)
					{
						ch->m_meteoSystem->GetMeteoActuel(ch);
					}
				}
			}
		}
	}
}

void Meteo::SelectionMeteo(LPCHARACTER ch)
{
	for (int i = 0; i < type_meteo_vector.size(); i++)
	{
		time_t now;
		struct tm *now_tm;
		int heure;
		int minute;
		now = time(NULL);
		now_tm = localtime(&now);
		heure = now_tm->tm_hour;

		std::string heureTemp = "";
		std::ostringstream oss;
		oss << heure;
		heureTemp += oss.str();
		oss.clear();
		if (heure_meteo_vector[i] == heureTemp)
		{
			environnementActuel = environnement_meteo_vector[i];
			if (musiqueActuel != musique_meteo_vector[i])
			{ 
				musiqueActuel = musique_meteo_vector[i];
			}
			if (meteoActuel != type_meteo_vector[i])
			{
				meteoActuel = type_meteo_vector[i];
				MSJMeteo = false;
				if (type_meteo_vector[i] == "pluie" && pluieActiver == false)
				{
					pluieActiver = true;
				}
				if (type_meteo_vector[i] == "neige" && neigeActiver == false)
				{
					neigeActiver = true;
				}
			}
			if (commandeActuel != commande_meteo_vector[i])
			{
				meteoActuel = type_meteo_vector[i];
				MSJMeteo = false;
			}
		}
	}
}

void Meteo::LoginMeteo(LPCHARACTER ch)
{
	if(m_pkMeteoSystemeUpdateEvent == NULL)
	{
		meteosysteme_event_info* info = AllocEventInfo<meteosysteme_event_info>();

		info->ch = ch;
		m_pkMeteoSystemeUpdateEvent = event_create(meteosysteme_update_event, info, PASSES_PER_SEC(1));
	}
}

void Meteo::GetMeteoActuel(LPCHARACTER ch)
{
	if(meteoActuel != "pluie" && pluieActiver)
	{
		pluieActiver = false;
		ch->ChatPacket(CHAT_TYPE_COMMAND, "rain d");
	}
	if(meteoActuel == "pluie" && pluieActiver)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "rain a");
	}
	if(meteoActuel != "neige" && neigeActiver)
	{
		neigeActiver = false;
		ch->ChatPacket(CHAT_TYPE_COMMAND, "xmas_snow 0");
	}
	if(meteoActuel == "neige" && neigeActiver)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "xmas_snow 1");
	}
	if (environnementActuel != "")
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "environnement %s", environnementActuel.c_str());
	}
	if (musiqueActuel != "")
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "musique %s", musiqueActuel.c_str());
		musiqueActuel = "";
	}
	MSJMeteo = true;
}

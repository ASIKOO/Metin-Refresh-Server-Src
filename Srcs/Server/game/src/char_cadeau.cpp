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
#include "char_cadeau.h"
#include "db.h"
#include <sstream>
#include <iostream>
#include <string>


using namespace std;

std::vector<int> liste_vnum_cadeau_vector;
std::vector<int> liste_vnum_cadeau_count_vector;

bool ChargementCadeau()
{
	if (liste_vnum_cadeau_vector.size() > 0)
	{
		liste_vnum_cadeau_vector.clear();
		liste_vnum_cadeau_count_vector.clear();
	}
	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("SELECT * FROM player_liste_cadeau%s;", get_table_postfix()));
	if(msg->uiSQLErrno != 0)
	{
		sys_err("Impossible de charger la liste des cadeaux");
		return false;
	}

	liste_vnum_cadeau_vector.reserve(msg->Get()->uiNumRows);
	liste_vnum_cadeau_count_vector.reserve(msg->Get()->uiNumRows);
	if (msg->Get()->uiNumRows > 0)
	{
		MYSQL_ROW row;
		int tempVnum;
		while ((row = mysql_fetch_row(msg->Get()->pSQLResult)))
		{
			str_to_number(tempVnum, row[1]);
			liste_vnum_cadeau_vector.emplace_back(tempVnum);
			str_to_number(tempVnum, row[2]);
			liste_vnum_cadeau_count_vector.emplace_back(tempVnum);
		}
		return true;
	}
	return false;
}


void SystemeCadeau::ReceptionCadeau(int idCarte, LPCHARACTER ch)
{
	if (idCarte > 2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Id de la carte incorrecte");
		return;
	}
	else
	{


		if (ch->m_CadeauSystem->dateCadeau+84600 <= get_global_time())
		{
			if (ch->m_CadeauSystem->carteCadeau[idCarte] > 0)
			{
				int count = liste_vnum_cadeau_count_vector[ch->m_CadeauSystem->carteCadeau[idCarte]];
				int vnum = liste_vnum_cadeau_vector[ch->m_CadeauSystem->carteCadeau[idCarte]];
				ch->m_CadeauSystem->dateCadeau = get_global_time();
				std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("UPDATE player_cadeau_statut%s set date ='%d' WHERE id_compte='%d';", get_table_postfix(), get_global_time(), ch->GetAID()));
				LPITEM item = ch->AutoGiveItem(vnum, count);
				ch->ChatPacket(CHAT_TYPE_COMMAND, "GuiCarteCadeauRecu");
			}
			else
			{
				ch->ChatPacket(CHAT_TYPE_COMMAND, "TitreMessage Dommage-vous-n'avez-reçu-aucun-cadeau-aujourd'hui.");
				ch->m_CadeauSystem->dateCadeau = get_global_time();
				std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("UPDATE player_cadeau_statut%s set date ='%d' WHERE id_compte='%d';", get_table_postfix(), get_global_time(), ch->GetAID()));

			}
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, "GuiCarteCadeauAvertissement");
			ch->ChatPacket(CHAT_TYPE_INFO, "Vous devez attendre pour recevoir votre prochain cadeau");
		}
	}
}

void SystemeCadeau::ExecutionCadeau(LPCHARACTER ch)
{
	std::string macJoueur;
	SQLMsg * pMsgTABLEMAC = DBManager::instance().DirectQuery("SELECT mac FROM account_alpha.account WHERE id='%d';", ch->GetAID());
	if (pMsgTABLEMAC->Get()->uiNumRows > 0)
	{
		MYSQL_ROW rowMAC = mysql_fetch_row(pMsgTABLEMAC->Get()->pSQLResult);
		macJoueur = rowMAC[0];
	}

	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("SELECT * FROM player_cadeau_statut%s WHERE id_compte='%d';", get_table_postfix(), ch->GetAID()));
	if(msg->Get()->uiNumRows <= 0)
	{
		std::unique_ptr<SQLMsg> msgInsert(DBManager::instance().DirectQuery("INSERT INTO player_cadeau_statut%s (id_compte, date, mac) VALUES('%d', '%u', '%s');", get_table_postfix(), ch->GetAID(), get_global_time(), macJoueur.c_str()));
		int nombreCadeau = liste_vnum_cadeau_vector.size();
		srand(time(0));
		int randomVide = rand() % 3 + 1;
		if (randomVide == 1)
		{
			ch->m_CadeauSystem->carteCadeau[0] = 0;
		
		}
		else
		{
			srand(time(0));
			ch->m_CadeauSystem->carteCadeau[0] = rand() % nombreCadeau;
		}
		if (randomVide == 2)
		{
			ch->m_CadeauSystem->carteCadeau[1] = 0;
		
		}
		else
		{
			srand(time(0));
			ch->m_CadeauSystem->carteCadeau[1] = rand() % nombreCadeau;
		}
		if (randomVide == 3)
		{
			ch->m_CadeauSystem->carteCadeau[2] = 0;
		
		}
		else
		{
			srand(time(0));
			ch->m_CadeauSystem->carteCadeau[2] = rand() % nombreCadeau;
		}
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenGuiCarteCadeau");
	}
	if (msg->Get()->uiNumRows > 0)
	{
		MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult);
		str_to_number(ch->m_CadeauSystem->dateCadeau, row[2]);
		std::unique_ptr<SQLMsg> msg2(DBManager::instance().DirectQuery("UPDATE player_cadeau_statut%s SET mac = '%s' WHERE id_compte = '%d'", get_table_postfix(), macJoueur.c_str(), ch->GetAID()));

	}

	if (ch->m_CadeauSystem->dateCadeau+84600 <= get_global_time())
	{
		bool macExistant = false;
		bool dateExistante = false;
		SQLMsg * pMsgTABLEMAC2 = DBManager::instance().DirectQuery("SELECT id_compte, mac, date FROM player_cadeau_statut%s WHERE mac='%s';", get_table_postfix(), macJoueur.c_str());
		if (pMsgTABLEMAC2->Get()->uiNumRows > 0)
		{
			MYSQL_ROW rowMAC2;
			while((rowMAC2 = mysql_fetch_row(pMsgTABLEMAC2->Get()->pSQLResult)))
			{
				int tempId= 0;
				str_to_number(tempId, rowMAC2[1]);
				if(macJoueur == rowMAC2[1] && tempId != ch->GetAID())
				{
					macExistant = true;
					int tempDate = 0;
					str_to_number(tempDate, rowMAC2[2]);
					if (tempDate+84600 >= get_global_time())
					{
						dateExistante = true;
					}
				}

				
			}
		}
		if (dateExistante == false)
		{
			int nombreCadeau = liste_vnum_cadeau_vector.size();
			srand(time(0));
			int randomVide = rand() % 3 + 1;
			if (randomVide == 1)
			{
				ch->m_CadeauSystem->carteCadeau[0] = 0;
			}
			else
			{
				srand(time(0));
				ch->m_CadeauSystem->carteCadeau[0] = rand() % nombreCadeau;
			}
			if (randomVide == 2)
			{
				ch->m_CadeauSystem->carteCadeau[1] = 0;
			}
			else
			{
				srand(time(0));
				ch->m_CadeauSystem->carteCadeau[1] = rand() % nombreCadeau;
			}
			if (randomVide == 3)
			{
				ch->m_CadeauSystem->carteCadeau[2] = 0;
		
			}
			else
			{
				srand(time(0));
				ch->m_CadeauSystem->carteCadeau[2] = rand() % nombreCadeau;
			}
			ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenGuiCarteCadeau");
		}
		else
		{
			ch->m_CadeauSystem->carteCadeau[0] = 0;
			ch->m_CadeauSystem->carteCadeau[1] = 0;
			ch->m_CadeauSystem->carteCadeau[2] = 0;
			std::unique_ptr<SQLMsg> msg3(DBManager::instance().DirectQuery("UPDATE player_cadeau_statut%s SET date = '%d' WHERE mac = '%s'", get_table_postfix(), get_global_time(), macJoueur.c_str()));

		}
	}
}


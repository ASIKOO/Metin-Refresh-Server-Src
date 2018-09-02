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
#include "char_inventaire.h"
#include "db.h"
#include "entity.h"
#include <sstream>
#include <iostream>
#include <string>

using namespace std;

void CharInventaire::ChargementInventaire(LPCHARACTER ch)
{
	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("SELECT * FROM player_inventaire%s WHERE id_player='%d';", get_table_postfix(), ch->GetPlayerID()));
	if(msg->Get()->uiNumRows <= 0)
	{
		std::unique_ptr<SQLMsg> msgInsert(DBManager::instance().DirectQuery("INSERT INTO player_inventaire%s (id_player) VALUES('%d');", get_table_postfix(), ch->GetPlayerID()));
		ch->ChatPacket(CHAT_TYPE_COMMAND, "AddInventaire %d", 18); // envoi le verrouillage du slot.
	}
	if (msg->Get()->uiNumRows > 0)
	{
		MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult);

		str_to_number(inventaire_joueur, row[2]);
		int tempInventaire = 18 - inventaire_joueur;
		ch->ChatPacket(CHAT_TYPE_COMMAND, "AddInventaire %d", tempInventaire); // envoi le verrouillage du slot.

	}
}

int CharInventaire::GetMaxPlaceInventaire()
{
	return (90 + (inventaire_joueur * 9));
}

bool CharInventaire::GetInventaire(LPCHARACTER ch, DWORD pos, DWORD size)
{
	
	if (pos <= 89) // si la position est compris entre l'inventaire 1 et 2
	{
		return true;
	}
	else
	{
		if (inventaire_joueur == 0) // si le joueur ne possède aucune extension.
		{
			return false;
		}
		if (inventaire_joueur == MAX_INVENTAIRE) // si le joueur possède toute les extensions.
		{
			return true;
		}
		int posMax = (inventaire_joueur * 9) + (90 - 5);
		if (pos > posMax)
		{
			return false;
		}
		if (pos <= posMax)
		{
			if (size == 1)
			{
				return true;
			}
			if (size > 1 && (pos+5 >= posMax))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "La taille de votre objet dépasse la taille deverrouillé.");
				return false;
			}
			if (size > 1 && (pos+10 >= posMax))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "La taille de votre objet dépasse la taille deverrouillé.");
				return false;
			}
			if (size > 1 && (pos+15 >= posMax))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "La taille de votre objet dépasse la taille deverrouillé.");
				return false;
			}

			return true;
		}
	}
}

bool CharInventaire::UnlockInventaire(LPCHARACTER ch)
{
	if (inventaire_joueur >= MAX_INVENTAIRE)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Vous avez déjà déverrouillé tout les inventaires.");
		return false;
	}
	else
	{
		std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("UPDATE player_inventaire%s set nombreInventaire=nombreInventaire+1 WHERE id_player='%d';", get_table_postfix(), ch->GetPlayerID()));
		inventaire_joueur = inventaire_joueur+1;
		ch->ChatPacket(CHAT_TYPE_INFO, "Un inventaire viens d'être débloqué.");
		ChargementInventaire(ch);
		return true;
	}
}

	
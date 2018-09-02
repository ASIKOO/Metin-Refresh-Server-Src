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
#include "char_magasinier.h"
#include "db.h"
#include <sstream>
#include <iostream>
#include <string>


using namespace std;

void Magasinier::Open(LPCHARACTER ch, int type, std::string pseudo)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "open_magasinier_gui");
	RefreshMagasinier(ch, type, pseudo);
}

void Magasinier::GetMagasinier(LPCHARACTER ch, int slot, int type, int slotDest)
{
	SQLMsg * pMsgTABLEMAGASINIER;
	std::string typeStockage = "";
	if (type == 1)
	{
		typeStockage = "SAFEBOX";
	}
	else if (type == 2)
	{
		typeStockage = "MALL";
	}
	else if (type == 3)
	{
		typeStockage = "GUILD";
	}
	else
	{
		return;
	}
 	pMsgTABLEMAGASINIER = DBManager::instance().DirectQuery("SELECT * FROM player_item_magasinier%s WHERE owner_id='%d' and window='%s' and pos='%d'", get_table_postfix(), ch->GetAID(), typeStockage.c_str(), slot);

	if (pMsgTABLEMAGASINIER == NULL)
	{
		return;
	}
	if (pMsgTABLEMAGASINIER->Get()->uiNumRows > 0)
	{
		MYSQL_ROW rowMAGA = mysql_fetch_row(pMsgTABLEMAGASINIER->Get()->pSQLResult);
		
		LPITEM item;
			
		item = ITEM_MANAGER::instance().CreateItem(IntMagasinierParser(rowMAGA[5]), IntMagasinierParser(rowMAGA[4]));
		
		//ch->ChatPacket(CHAT_TYPE_INFO, "ID item %d", IntMagasinierParser(rowMAGA[5]));
		if (!item)
			return;

		int iEmptyPos = ch->GetEmptyInventory(item->GetSize());

		if (iEmptyPos < 0)
		{
			M2_DESTROY_ITEM(item);
			return;
		}

		long tableSockets[3] = {IntMagasinierParser(rowMAGA[7]), IntMagasinierParser(rowMAGA[8]), IntMagasinierParser(rowMAGA[9])};
		TPlayerItemAttribute tableAttrs[15] = {{IntMagasinierParser(rowMAGA[13]), IntMagasinierParser(rowMAGA[14])}, {IntMagasinierParser(rowMAGA[15]), IntMagasinierParser(rowMAGA[16])}, {IntMagasinierParser(rowMAGA[17]), IntMagasinierParser(rowMAGA[18])}, {IntMagasinierParser(rowMAGA[19]), IntMagasinierParser(rowMAGA[20])}, {IntMagasinierParser(rowMAGA[21]), IntMagasinierParser(rowMAGA[22])}, {IntMagasinierParser(rowMAGA[23]), IntMagasinierParser(rowMAGA[24])}, {IntMagasinierParser(rowMAGA[25]), IntMagasinierParser(rowMAGA[26])}, {IntMagasinierParser(rowMAGA[27]), IntMagasinierParser(rowMAGA[28])}, {IntMagasinierParser(rowMAGA[29]), IntMagasinierParser(rowMAGA[30])}, {IntMagasinierParser(rowMAGA[31]), IntMagasinierParser(rowMAGA[32])}, {IntMagasinierParser(rowMAGA[33]), IntMagasinierParser(rowMAGA[34])}, {IntMagasinierParser(rowMAGA[35]), IntMagasinierParser(rowMAGA[36])}, {IntMagasinierParser(rowMAGA[37]), IntMagasinierParser(rowMAGA[38])}, {IntMagasinierParser(rowMAGA[39]), IntMagasinierParser(rowMAGA[40])}, {IntMagasinierParser(rowMAGA[41]), IntMagasinierParser(rowMAGA[42])}};
		
		item->SetSockets(tableSockets);
		item->SetAttributes(tableAttrs);
		
#ifdef __CHANGE_LOOK_SYSTEM__
		item->SetItemLook(IntMagasinierParser(rowMAGA[6]));
#endif

		item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
		

		ITEM_MANAGER::instance().FlushDelayedSave(item);
		DBManager::instance().DirectQuery("DELETE FROM player_item_magasinier%s WHERE owner_id='%d' and window='%s' and pos='%d'", get_table_postfix(), ch->GetAID(), typeStockage.c_str(), slot);

		ch->Save();
	
	}	
	RefreshMagasinier(ch, type, "");
}


int Magasinier::IntMagasinierParser(std::string valeur)
{
	int valeurInt;
	str_to_number(valeurInt, valeur.c_str());
	return valeurInt;
}

void Magasinier::AddMagasinier(LPCHARACTER ch, int slot, int type, int slotDest) // Ajoute un objet dans l'entrepôt choisi.
{
	LPITEM item = ch->GetItem(TItemPos(INVENTORY, slot));
	if (item != NULL)
	{
		if(item->GetWindow() == INVENTORY && item->IsEquipped() == false)
		{ 
			std::string typeStockage = "";
			if (type == 1)
			{
				typeStockage = "SAFEBOX";
			}
			else if (type == 3)
			{
				typeStockage = "GUILD";
			}
			else
			{
				return;
			}
			DBManager::instance().DirectQuery("INSERT INTO player_item_magasinier%s (id, owner_id, window, pos, count, vnum, dwVnum, socket0, socket1, socket2, attrtype0, attrvalue0, attrtype1, attrvalue1, attrtype2, attrvalue2, attrtype3, attrvalue3, attrtype4, attrvalue4, attrtype5, attrvalue5, attrtype6, attrvalue6, applytype0, applyvalue0, applytype1, applyvalue1, applytype2, applyvalue2, applytype3, applyvalue3, applytype4, applyvalue4, applytype5, applyvalue5, applytype6, applyvalue6, applytype7, applyvalue7) VALUES(%u, %u, '%s', %d, %u, %u, %u, %ld, %ld, %ld, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)", get_table_postfix(), item->GetID(), ch->GetAID(), typeStockage.c_str(), slotDest, item->GetCount(), item->GetVnum(), item->GetItemLook(), item->GetSocket(0), item->GetSocket(1), item->GetSocket(2), item->GetAttributeType(0), item->GetAttributeValue(0), item->GetAttributeType(1), item->GetAttributeValue(1), item->GetAttributeType(2), item->GetAttributeValue(2), item->GetAttributeType(3), item->GetAttributeValue(3), item->GetAttributeType(4), item->GetAttributeValue(4), item->GetAttributeType(5), item->GetAttributeValue(5), item->GetAttributeType(6), item->GetAttributeValue(6), item->GetAttributeType(7), item->GetAttributeValue(7), item->GetAttributeType(8), item->GetAttributeValue(8), item->GetAttributeType(9), item->GetAttributeValue(9), item->GetAttributeType(10), item->GetAttributeValue(10), item->GetAttributeType(11), item->GetAttributeValue(11), item->GetAttributeType(12), item->GetAttributeValue(12), item->GetAttributeType(13), item->GetAttributeValue(13), item->GetAttributeType(14), item->GetAttributeValue(14));
			ITEM_MANAGER::instance().RemoveItem(item);

		}
		RefreshMagasinier(ch, type, "");
		return;	
	}
	else
	{
		return;
	}

}

void Magasinier::RefreshMagasinier(LPCHARACTER ch, int type, std::string pseudo)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "refresh_magasinier_gui");
	ch->ChatPacket(CHAT_TYPE_COMMAND, "refresh_magasinier_guilde_gui");
	int idJoueur = ch->GetPlayerID();
	int idCompte = ch->GetAID();
	if (ch->IsGM()) // Mode Espion (Ne permet pas de prendre les items du joueur, seul une intervention sur la BDD est possible)
	{
		if (pseudo != "")
		{
			std::string tempPseudo = pseudo;
			char tempSqlPseudo[64+1];
			DBManager::instance().EscapeString(tempSqlPseudo, sizeof(tempSqlPseudo), tempPseudo.c_str(), tempPseudo.length());
			SQLMsg * pMsgTABLEJOUEUR = DBManager::instance().DirectQuery("SELECT * FROM player%s WHERE name='%s'", get_table_postfix(), tempSqlPseudo);
			if(pMsgTABLEJOUEUR->Get()->uiNumRows > 0)
			{
				MYSQL_ROW rowJoueur = mysql_fetch_row(pMsgTABLEJOUEUR->Get()->pSQLResult);
				str_to_number(idJoueur, rowJoueur[0]);
				str_to_number(idCompte, rowJoueur[1]);
			}
			else
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Le joueur %s n'existe pas.", tempSqlPseudo);
				return;
			}
		}
	}
	std::string typeStockage = "";
	SQLMsg * pMsgTABLEMAGASINIER;
	if (type <= 1) // entrepot
	{
		typeStockage = "SAFEBOX";
		pMsgTABLEMAGASINIER = DBManager::instance().DirectQuery("SELECT * FROM player_item_magasinier%s WHERE owner_id='%d' and window='%s'", get_table_postfix(), idCompte, typeStockage.c_str());
	}
	else if (type == 2) // itemshop
	{
		typeStockage = "MALL";
		pMsgTABLEMAGASINIER = DBManager::instance().DirectQuery("SELECT * FROM player_item_magasinier%s WHERE owner_id='%d' and window='%s'", get_table_postfix(), idCompte, typeStockage.c_str());
	}
	else if (type == 3) // Guilde
	{
		typeStockage = "GUILDE";
		pMsgTABLEMAGASINIER = DBManager::instance().DirectQuery("SELECT * FROM player_item_magasinier%s WHERE owner_id='%d' and window='%s'", get_table_postfix(), ch->GetGuild()->GetID(), typeStockage.c_str());
	}
	
	if (pMsgTABLEMAGASINIER->Get()->uiNumRows > 0)
	{
		MYSQL_ROW rowMAGA;
		while((rowMAGA = mysql_fetch_row(pMsgTABLEMAGASINIER->Get()->pSQLResult)))
		{
			std::string requeteItem;
			std::ostringstream oss;
			for (int i = 0; i < 42; i++)
			{
				oss << rowMAGA[i];
				oss << "|";
			}
			requeteItem = oss.str();
			oss.clear();
			ch->ChatPacket(CHAT_TYPE_COMMAND, "magasinier_item_data %s", requeteItem.c_str());
		}
	}
	else
	{
		return;
	}
}

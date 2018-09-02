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
#include "char_offlineshop.h"
#include "db.h"
#include <sstream>
#include <iostream>
#include <string>


using namespace std;

std::vector<int> liste_vnum_shop_vector;
std::vector<std::string> liste_nom_shop_vector;

bool ShopProto() // Charge le shop proto
{
	if (liste_vnum_shop_vector.size() > 0)
	{
		liste_vnum_shop_vector.clear();
		liste_nom_shop_vector.clear();
	}

	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("SELECT * FROM shop_proto%s;", get_table_postfix()));
	if(msg->uiSQLErrno != 0)
	{
		sys_err("Impossible de charger la liste des vnums de shop");
		return false;
	}

	liste_vnum_shop_vector.reserve(msg->Get()->uiNumRows);
	liste_nom_shop_vector.reserve(msg->Get()->uiNumRows);
	if (msg->Get()->uiNumRows > 0)
	{
		MYSQL_ROW row;
		int tempVnum;
		while ((row = mysql_fetch_row(msg->Get()->pSQLResult)))
		{
			liste_nom_shop_vector.emplace_back(row[1]);
			str_to_number(tempVnum, row[2]);
			liste_vnum_shop_vector.emplace_back(tempVnum);
		}
		return true;
	}
	return false;
}

void RespawnShop() // Fait réapparaitre les shops
{
	char szQuery1[1024+1];
	snprintf(szQuery1, sizeof(szQuery1), "SELECT * FROM player%s;", get_table_postfix());
	std::unique_ptr<SQLMsg> msgJoueur(DBManager::instance().DirectQuery(szQuery1));
	MYSQL_ROW rowJoueur;
	while((rowJoueur = mysql_fetch_row(msgJoueur->Get()->pSQLResult)))
	{

		const char* pseudoJoueur = rowJoueur[2];
		int idJoueur = 0;
		str_to_number(idJoueur, rowJoueur[0]);
		char szQuery[1024+1];
		snprintf(szQuery, sizeof(szQuery), "SELECT * FROM private_herra_shops%s WHERE status = '%d' AND owner_name='%s';", get_table_postfix(), 1, pseudoJoueur);
		std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery(szQuery));
		if (msg->Get()->uiNumRows > 0)
		{
			MYSQL_ROW row;
			while ((row = mysql_fetch_row(msg->Get()->pSQLResult)))
			{
				int shopID;
				int channelID;
				int map_index;
				int vnum_shop;
				str_to_number(shopID, row[1]);
				str_to_number(map_index, row[6]);
				str_to_number(channelID, row[10]);
				str_to_number(vnum_shop, row[11]);
				if (g_bChannel == channelID && map_allow_find(map_index) == true)
				{
					int randomID = number(10000, 1000000);
					SQLMsg * pMsgTABLESHOP = DBManager::instance().DirectQuery("UPDATE private_herra_shops%s SET shop_id='%d' WHERE owner_name='%s' AND status='%d'", get_table_postfix(), randomID, pseudoJoueur, 1);
					SQLMsg * pMsgTABLESHOPITEM = DBManager::instance().DirectQuery("UPDATE private_herra_items%s SET shop_id='%d' WHERE owner_id='%d'", get_table_postfix(), randomID, idJoueur);
				}
			}
		}
		CShopManager::instance().Herra_RespawnShops(pseudoJoueur, idJoueur, 84600); // Fait apparaître à nouveau le shop.
	}
}


void OfflineShop::OpenShop(LPCHARACTER ch) // Execute l'ouverture de l'interface de shop.
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "open_shop_gui");
	RefreshShop(ch);
}

//Récupère l'argent contenu dans le shop.
void OfflineShop::GetYang(LPCHARACTER ch, int shopID)
{
	SQLMsg * pMsgTABLESHOPPRIX = DBManager::instance().DirectQuery("SELECT price, won from private_herra_items%s WHERE shop_id = '%d' AND owner_id = '%d' AND status = '1';", get_table_postfix(), shopID, ch->GetPlayerID());
	if (pMsgTABLESHOPPRIX->Get()->uiNumRows > 0)
	{
		MYSQL_ROW rowMONNAIE;
		long long tempSomme = 0;
		long long sommeTotal = 0;
#ifdef ENABLE_CHEQUE_SYSTEM
		int sommeWon = 0;
		int tempWon = 0;
#endif
		while((rowMONNAIE = mysql_fetch_row(pMsgTABLESHOPPRIX->Get()->pSQLResult)))
		{
			str_to_number(tempSomme, rowMONNAIE[0]);
#ifdef ENABLE_CHEQUE_SYSTEM
			str_to_number(tempWon, rowMONNAIE[1]);
			sommeWon += tempWon;
#endif
			sommeTotal += tempSomme;
		}
		
		ch->PointChange(POINT_GOLD, sommeTotal, true);
#ifdef ENABLE_CHEQUE_SYSTEM
		if (sommeWon <= 0 && sommeTotal > 0)
		{
			DBManager::instance().DirectQuery("DELETE FROM private_herra_items%s WHERE shop_id = '%d' AND owner_id = '%d' AND status = '1';", get_table_postfix(), shopID, ch->GetPlayerID());
		}
		else
		{
			DBManager::instance().DirectQuery("UPDATE private_herra_items%s set price=0 WHERE shop_id = '%d' AND owner_id = '%d' AND status = '1';", get_table_postfix(), shopID, ch->GetPlayerID());
		}
#else
		DBManager::instance().DirectQuery("DELETE FROM private_herra_items%s WHERE shop_id = '%d' AND owner_id = '%d' AND status = '1';", get_table_postfix(), shopID, ch->GetPlayerID());
#endif
	}
	delete pMsgTABLESHOPPRIX;
	RefreshItem(ch, shopID);
}

#ifdef ENABLE_CHEQUE_SYSTEM

//Récupère les wons contenu dans le shop.
void OfflineShop::GetWon(LPCHARACTER ch, int shopID)
{
	SQLMsg * pMsgTABLESHOPPRIX = DBManager::instance().DirectQuery("SELECT won, price from private_herra_items%s WHERE shop_id = '%d' AND owner_id = '%d' AND status = '1';", get_table_postfix(), shopID, ch->GetPlayerID());
	if (pMsgTABLESHOPPRIX->Get()->uiNumRows > 0)
	{
		MYSQL_ROW rowWON;
		int tempSomme = 0;
		int sommeTotal = 0;
		int tempGold = 0;
		int sommeGold = 0;
		while((rowWON = mysql_fetch_row(pMsgTABLESHOPPRIX->Get()->pSQLResult)))
		{
			str_to_number(tempSomme, rowWON[0]);
			str_to_number(tempGold, rowWON[0]);
			sommeGold += tempGold;
			sommeTotal += tempSomme;
		}
		ch->PointChange(POINT_CHEQUE, sommeTotal, false);
		if (sommeGold <= 0 && sommeTotal > 0)
		{
			DBManager::instance().DirectQuery("DELETE FROM private_herra_items%s WHERE shop_id = '%d' AND owner_id = '%d' AND status = '1';", get_table_postfix(), shopID, ch->GetPlayerID());
		}
		else
		{
			DBManager::instance().DirectQuery("UPDATE private_herra_items%s set won=0 WHERE shop_id = '%d' AND owner_id = '%d' AND status = '1';", get_table_postfix(), shopID, ch->GetPlayerID());
		}
	}
	delete pMsgTABLESHOPPRIX;
	RefreshItem(ch, shopID);
}

#endif

// Récupère l'item d'un shop.
void OfflineShop::GetItem(LPCHARACTER ch, int shopID, int posItem) 
{
	SQLMsg * pMsgTABLESHOPITEM = DBManager::instance().DirectQuery("SELECT * from private_herra_items%s WHERE shop_id = '%d' AND owner_id = '%d' AND pos= '%d';", get_table_postfix(), shopID, ch->GetPlayerID(), posItem);
	SQLMsg * pMsgTABLESHOPITEMNOMBRE = DBManager::instance().DirectQuery("SELECT COUNT(*) from private_herra_items%s WHERE shop_id = '%d' AND owner_id = '%d';", get_table_postfix(), shopID, ch->GetPlayerID());

	int nombreItem = 0;
	if ( pMsgTABLESHOPITEMNOMBRE->Get()->uiNumRows > 0 )
	{
		MYSQL_ROW row = mysql_fetch_row(pMsgTABLESHOPITEMNOMBRE->Get()->pSQLResult);

		int	count = 0;
		str_to_number(nombreItem, row[0]);
	}
	if (pMsgTABLESHOPITEM->Get()->uiNumRows > 0)
	{
		MYSQL_ROW rowITEM;
		int tempNombre = 0;
		rowITEM = mysql_fetch_row(pMsgTABLESHOPITEM->Get()->pSQLResult);
		
		str_to_number(tempNombre, rowITEM[27]); // statut 
		if (tempNombre >= 1)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Vous avez vendu cet objet à %s.", rowITEM[28]);
			return;
		}
		else
		{
			int tempVnum = 0;
			int tempQuantiter = 0;
			int socket0;
			int socket1;
			int socket2;
			int attrtype0;
			int attrvalue0;
			int attrtype1;
			int attrvalue1;
			int attrtype2;
			int attrvalue2;
			int attrtype3;
			int attrvalue3;
			int attrtype4;
			int attrvalue4;
			int attrtype5;
			int attrvalue5;
			int attrtype6;
			int attrvalue6;
			int dwVnum;
			str_to_number(tempVnum, rowITEM[4]);
			if (tempVnum > 0)
			{
				if (ch->GetEmptyInventory(tempVnum))
				{
					str_to_number(tempQuantiter, rowITEM[3]);
					str_to_number(socket0, rowITEM[5]); // socket0
					str_to_number(socket1, rowITEM[6]); // socket1
					str_to_number(socket2, rowITEM[7]); // socket2
					str_to_number(attrtype0, rowITEM[11]); // attrtype0
					str_to_number(attrvalue0, rowITEM[12]); // attrvalue0
					str_to_number(attrtype1, rowITEM[13]); // attrtype1
					str_to_number(attrvalue1, rowITEM[14]); // attrvalue1
					str_to_number(attrtype2, rowITEM[15]); // attrtype2
					str_to_number(attrvalue2, rowITEM[16]); // attrvalue2
					str_to_number(attrtype3, rowITEM[17]); // attrtype3
					str_to_number(attrvalue3, rowITEM[18]); // attrvalue3
					str_to_number(attrtype4, rowITEM[19]); // attrtype4
					str_to_number(attrvalue4, rowITEM[20]); // attrvalue4
					str_to_number(attrtype5, rowITEM[21]); // attrtype5
					str_to_number(attrvalue5, rowITEM[22]); // attrvalue5
					str_to_number(attrtype6, rowITEM[23]); // attrtype6
					str_to_number(attrvalue6, rowITEM[24]); // attrvalue6
					str_to_number(dwVnum, rowITEM[47]); // dwVnum permutation
					LPITEM item = ch->AutoGiveItem(tempVnum, tempQuantiter);
					item->SetSocket(0, socket0);
					item->SetSocket(1, socket1);
					item->SetSocket(2, socket2);
					item->SetForceAttribute(0, attrtype0, attrvalue0);
					item->SetForceAttribute(1, attrtype1, attrvalue1);
					item->SetForceAttribute(2, attrtype2, attrvalue2);
					item->SetForceAttribute(3, attrtype3, attrvalue3);
					item->SetForceAttribute(4, attrtype4, attrvalue4);
					item->SetForceAttribute(5, attrtype5, attrvalue5);
					item->SetForceAttribute(6, attrtype6, attrvalue6);
					item->SetItemLook(dwVnum);
					DBManager::instance().DirectQuery("DELETE from private_herra_items%s WHERE shop_id = '%d' AND owner_id = '%d' AND pos = '%d';", get_table_postfix(), shopID, ch->GetPlayerID(), posItem);
					nombreItem = nombreItem - 1;
					if (ch && CHARACTER_MANAGER::instance().Find(shopID))
					{
						LPCHARACTER npc = CHARACTER_MANAGER::instance().Find(shopID);
						if (nombreItem <= 0)
						{
							FermerShop(ch, shopID);
						}
					}
				}
				else
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Vous n'avez pas assez de place dans votre inventaire.");
					return;
				}
			}
		}
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Cet objet est introuvable.");
	}
	RefreshItem(ch, shopID);
}

void OfflineShop::RefreshItem(LPCHARACTER ch, int shopId) // Rafraichie les objets du joueur.
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "shop_gui_clear_items_data");
	SQLMsg * pMsgTABLESHOPPRIX = DBManager::instance().DirectQuery("SELECT price from private_herra_items%s WHERE shop_id = '%d' AND owner_id = '%d' AND status = '1';", get_table_postfix(), shopId, ch->GetPlayerID());
	if (pMsgTABLESHOPPRIX->Get()->uiNumRows > 0)
	{
		MYSQL_ROW rowMONNAIE;
		long long tempSomme = 0;
		long long sommeTotal = 0;
		while((rowMONNAIE = mysql_fetch_row(pMsgTABLESHOPPRIX->Get()->pSQLResult)))
		{
			str_to_number(tempSomme, rowMONNAIE[0]);
			sommeTotal += tempSomme;
		}
		ch->ChatPacket(CHAT_TYPE_COMMAND, "shop_gui_set_data_earnings %lld", sommeTotal);

	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "shop_gui_set_data_earnings 0");
	}
	delete pMsgTABLESHOPPRIX;

#ifdef ENABLE_CHEQUE_SYSTEM
	SQLMsg * pMsgTABLESHOPWON = DBManager::instance().DirectQuery("SELECT won from private_herra_items%s WHERE shop_id = '%d' AND owner_id = '%d' AND status = '1';", get_table_postfix(), shopId, ch->GetPlayerID());
	if (pMsgTABLESHOPWON->Get()->uiNumRows > 0)
	{
		MYSQL_ROW rowWON;
		int tempSomme = 0;
		int sommeTotal = 0;
		while((rowWON = mysql_fetch_row(pMsgTABLESHOPWON->Get()->pSQLResult)))
		{
			str_to_number(tempSomme, rowWON[0]);
			sommeTotal += tempSomme;
		}
		ch->ChatPacket(CHAT_TYPE_COMMAND, "shop_gui_set_data_won %d", sommeTotal);

	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "shop_gui_set_data_won 0");
	}
	delete pMsgTABLESHOPWON;
#endif

	SQLMsg * pMsgTABLESHOPITEM = DBManager::instance().DirectQuery("SELECT * from private_herra_items%s WHERE shop_id = '%d' AND owner_id = '%d';", get_table_postfix(), shopId, ch->GetPlayerID());

	if (pMsgTABLESHOPITEM->Get()->uiNumRows > 0)
	{
		MYSQL_ROW rowITEM;

		while((rowITEM = mysql_fetch_row(pMsgTABLESHOPITEM->Get()->pSQLResult)))
		{
			int tempNombre = 0;
			std::string requeteItem;
			std::ostringstream oss;
			str_to_number(tempNombre, rowITEM[2]); // position
			oss << tempNombre;
			oss << "|";
			str_to_number(tempNombre, rowITEM[4]); // vnum
			oss << tempNombre;
			oss << "|";
			str_to_number(tempNombre, rowITEM[3]); // nombre
			oss << tempNombre;
			oss << "|";
			str_to_number(tempNombre, rowITEM[5]); // socket0
			oss << tempNombre;
			oss << "|";
			str_to_number(tempNombre, rowITEM[6]); // socket1
			oss << tempNombre;
			oss << "|";
			str_to_number(tempNombre, rowITEM[7]); // socket2
			oss << tempNombre;
			oss << "|";
			str_to_number(tempNombre, rowITEM[11]); // attrtype0
			oss << tempNombre;
			oss << "|";
			str_to_number(tempNombre, rowITEM[12]); // attrvalue0
			oss << tempNombre;
			oss << "|";
			str_to_number(tempNombre, rowITEM[13]); // attrtype1
			oss << tempNombre;
			oss << "|";
			str_to_number(tempNombre, rowITEM[14]); // attrvalue1
			oss << tempNombre;
			oss << "|";
			str_to_number(tempNombre, rowITEM[15]); // attrtype2
			oss << tempNombre;
			oss << "|";
			str_to_number(tempNombre, rowITEM[16]); // attrvalue2
			oss << tempNombre;
			oss << "|";
			str_to_number(tempNombre, rowITEM[17]); // attrtype3
			oss << tempNombre;
			oss << "|";
			str_to_number(tempNombre, rowITEM[18]); // attrvalue3
			oss << tempNombre;
			oss << "|";
			str_to_number(tempNombre, rowITEM[19]); // attrtype4
			oss << tempNombre;
			oss << "|";
			str_to_number(tempNombre, rowITEM[20]); // attrvalue4
			oss << tempNombre;
			oss << "|";
			str_to_number(tempNombre, rowITEM[21]); // attrtype5
			oss << tempNombre;
			oss << "|";
			str_to_number(tempNombre, rowITEM[22]); // attrvalue5
			oss << tempNombre;
			oss << "|";
			str_to_number(tempNombre, rowITEM[23]); // attrtype6
			oss << tempNombre;
			oss << "|";
			str_to_number(tempNombre, rowITEM[24]); // attrvalue6
			oss << tempNombre;
			oss << "|";
			str_to_number(tempNombre, rowITEM[25]); // prix
			oss << tempNombre;
			oss << "|";
			str_to_number(tempNombre, rowITEM[46]); // won
			oss << tempNombre;
			oss << "|";
			str_to_number(tempNombre, rowITEM[27]); // statut
			oss << tempNombre;
			oss << "|";
			oss << rowITEM[28]; // pseudo acheteur
			oss << "|";
			str_to_number(tempNombre, rowITEM[29]); // donnée achat.
			oss << tempNombre;
			oss << "|";



			int applytype0;
			int applyvalue0;
			int applytype1;
			int applyvalue1;
			int applytype2;
			int applyvalue2;
			int applytype3;
			int applyvalue3;
			int applytype4;
			int applyvalue4;
			int applytype5;
			int applyvalue5;
			int applytype6;
			int applyvalue6;
			int applytype7;
			int applyvalue7;
			int transmutation;
			str_to_number(applytype0, rowITEM[30]);
			str_to_number(applyvalue0, rowITEM[31]);
			str_to_number(applytype1, rowITEM[32]);
			str_to_number(applyvalue1, rowITEM[33]);
			str_to_number(applytype2, rowITEM[34]);
			str_to_number(applyvalue2, rowITEM[35]);
			str_to_number(applytype3, rowITEM[36]);
			str_to_number(applyvalue3, rowITEM[37]);
			str_to_number(applytype4, rowITEM[38]);
			str_to_number(applyvalue4, rowITEM[39]);
			str_to_number(applytype5, rowITEM[40]);
			str_to_number(applyvalue5, rowITEM[41]);
			str_to_number(applytype6, rowITEM[42]);
			str_to_number(applyvalue6, rowITEM[43]);
			str_to_number(applytype7, rowITEM[44]);
			str_to_number(applyvalue7, rowITEM[45]);
			str_to_number(transmutation, rowITEM[47]);

			oss << applytype0;
			oss << "|";
			oss << applyvalue0;
			oss << "|";
			oss << applytype1;
			oss << "|";
			oss << applyvalue1;
			oss << "|";
			oss << applytype2;
			oss << "|";
			oss << applyvalue2;
			oss << "|";
			oss << applytype3;
			oss << "|";
			oss << applyvalue3;
			oss << "|";
			oss << applytype4;
			oss << "|";
			oss << applyvalue4;
			oss << "|";
			oss << applytype5;
			oss << "|";
			oss << applyvalue5;
			oss << "|";
			oss << applytype6;
			oss << "|";
			oss << applyvalue6;
			oss << "|";
			oss << applytype7;
			oss << "|";
			oss << applyvalue7;
			oss << "|";
			oss << transmutation;

			requeteItem = oss.str();
			replace(requeteItem.begin(), requeteItem.end(), " ", "_");
			ch->ChatPacket(CHAT_TYPE_COMMAND, "shop_gui_set_data_items %s", requeteItem.c_str());
			oss.clear();
		}

	}

}

void OfflineShop::RefreshShop(LPCHARACTER ch) // Rafraichie les shops du joueur.
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "shop_gui_clear_shops_data");

	SQLMsg * pMsgTABLESHOP = DBManager::instance().DirectQuery("SELECT * FROM private_herra_shops%s WHERE owner_id='%d'", get_table_postfix(), ch->GetPlayerID());
	if (pMsgTABLESHOP->Get()->uiNumRows > 0)
	{
		MYSQL_ROW rowSHOP;
		int compteurShop = 0;
		int shopID = 0;
		while((rowSHOP = mysql_fetch_row(pMsgTABLESHOP->Get()->pSQLResult)))
		{

			std::string donneeShop;
			std::string nomShopTemp = rowSHOP[5];
			replace(nomShopTemp.begin(), nomShopTemp.end(), ' ', '_');
			std::ostringstream oss;
			oss << "";
			oss << compteurShop;
			oss << "|";
			oss << rowSHOP[1];
			oss << "|";
			oss << rowSHOP[4];
			oss << "|";
			oss << nomShopTemp.c_str();
			oss << "|";
			oss << rowSHOP[9];
			donneeShop = oss.str();
			replace(donneeShop.begin(), donneeShop.end(), ' ', '_');
			ch->ChatPacket(CHAT_TYPE_COMMAND, "shop_gui_set_data_shops %s", donneeShop.c_str());
			str_to_number(shopID, rowSHOP[1]);
			if (compteurShop == 0)
			{
				RefreshItem(ch, shopID);
			}
			compteurShop++;

		}
	}
	delete pMsgTABLESHOP;
}

void OfflineShop::FermerShop(LPCHARACTER ch, int IDshop) // Ferme le shop.
{
	
	LPCHARACTER npc;
	
	if (ch && CHARACTER_MANAGER::instance().Find(IDshop))
	{
		npc = CHARACTER_MANAGER::instance().Find(IDshop);
		SQLMsg * pMsgTABLESHOP = DBManager::instance().DirectQuery("SELECT owner_name,status,channel FROM private_herra_shops%s WHERE shop_id='%d' AND owner_id='%d'", get_table_postfix(), IDshop, ch->GetPlayerID());
		if (pMsgTABLESHOP->Get()->uiNumRows > 0)
		{
			int tempStatus = 0;
			int channelID = 0;
			MYSQL_ROW rowStatutShop = mysql_fetch_row(pMsgTABLESHOP->Get()->pSQLResult);
			str_to_number(tempStatus, rowStatutShop[1]);
			str_to_number(channelID, rowStatutShop[2]);
			if (tempStatus == 1)
			{
				if (g_bChannel == channelID)
				{
					DBManager::instance().DirectQuery("UPDATE private_herra_shops%s SET time = UNIX_TIMESTAMP(), status = '2' WHERE shop_id = '%d' AND owner_id = '%d';", get_table_postfix(), (int)npc->GetVID(), ch->GetPlayerID());
					npc->CloseHerraShop();
					SQLMsg * pMsgTABLESHOPITEM = DBManager::instance().DirectQuery("SELECT * from private_herra_items%s WHERE shop_id = '%d' AND owner_id = '%d';", get_table_postfix(), IDshop, ch->GetPlayerID());

					if (pMsgTABLESHOPITEM->Get()->uiNumRows > 0)
					{
						MYSQL_ROW rowITEM;

						while((rowITEM = mysql_fetch_row(pMsgTABLESHOPITEM->Get()->pSQLResult)))
						{
							int pos = 0;
							str_to_number(pos, rowITEM[2]); // position
							GetItem(ch, IDshop, pos);
						}
						GetYang(ch, IDshop);
						GetWon(ch, IDshop);
						RefreshShop(ch);
					}
				}
				else
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Vous devez être channel %d pour fermer votre magasin.", channelID);
				}
			}
			else
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Votre magasin est déjà fermé.");
			}
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Ce n'est pas votre magasin.");
		}
		delete pMsgTABLESHOP;
	}
	else
	{
		SQLMsg * pMsgTABLESHOP = DBManager::instance().DirectQuery("SELECT owner_name,status,channel FROM private_herra_shops%s WHERE shop_id='%d' AND owner_id='%d'", get_table_postfix(), IDshop, ch->GetPlayerID());
		if (pMsgTABLESHOP->Get()->uiNumRows > 0)
		{
			int tempStatus = 0;
			int channelID = 0;
			MYSQL_ROW rowStatutShop = mysql_fetch_row(pMsgTABLESHOP->Get()->pSQLResult);
			str_to_number(tempStatus, rowStatutShop[1]);
			str_to_number(channelID, rowStatutShop[2]);
			if (tempStatus == 1)
			{
				if (g_bChannel == channelID)
				{
					DBManager::instance().DirectQuery("UPDATE private_herra_shops%s SET time = UNIX_TIMESTAMP(), status = '2' WHERE shop_id = '%d' AND owner_id = '%d';", get_table_postfix(), (int)npc->GetVID(), ch->GetPlayerID());
				}
				else
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Vous devez être channel %d pour fermer votre magasin.", channelID);
				}
			}
			else
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Votre magasin est déjà fermé.");
			}
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Ce n'est pas votre magasin.");
		}
		delete pMsgTABLESHOP;
	}
	RefreshShop(ch);
}

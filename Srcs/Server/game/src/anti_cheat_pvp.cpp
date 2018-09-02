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
#include "anti_cheat_pvp.h"
#include "db.h"

void AntiCheat::ChargementConfigAntiCheat()
{
	SQLMsg * pMsg = DBManager::instance().DirectQuery("select * from config_anti_cheat_pvp%s", get_table_postfix());
	if (pMsg->Get()->uiNumRows > 0)
	{
		MYSQL_ROW  row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		str_to_number(tempsMinimum, row[0]);
		str_to_number(detectionMax, row[1]);
		str_to_number(detectionAvertissement, row[2]);
		str_to_number(detectionBan, row[3]);
		str_to_number(recordBanMax, row[4]);
		str_to_number(tempsClear, row[5]);
		ConfigAntiCheat = 1;
	}
	else
	{
		tempsMinimum = 10; // Temps en seconde de détection
		detectionMax = 10; // Détection max avant banissement
		detectionAvertissement = 15; // Détection avertissement
		detectionBan = 20; // Détection ban
		recordBanMax = 5; // Record de ban max
		tempsClear = 15; // Temps de nettoyage en minute.
		ConfigAntiCheat = 1; // Vérifie que la configuration est chargé.
	}
}

void AntiCheat::AntiCheatPvP(LPCHARACTER tueur, LPCHARACTER tuer)
{
	if (Statut <= 0)
	{
		if (ConfigAntiCheat <= 0)
		{
			ChargementConfigAntiCheat();
		}
		UpdateDB("logkill", tueur, tuer, 0, 0);
		char szQuery[1024+1];
		snprintf(szQuery, sizeof(szQuery), "SELECT * FROM anti_cheat_pvp%s WHERE tueur = '%s';", get_table_postfix(), tueur->GetName());
		std::unique_ptr<SQLMsg> msgKill(DBManager::instance().DirectQuery(szQuery));
		if (msgKill->Get()->uiNumRows > 0)
		{
			MYSQL_ROW row;
			while((row = mysql_fetch_row(msgKill->Get()->pSQLResult)))
			{
				int dateKill;
				str_to_number(dateKill, row[3]);
				int dateActuel = GetDateActuel();
				if ((dateKill + (tempsClear * 60)) < dateActuel)
				{
					detectionTemp = 0;
					derniereDate = 0;
					UpdateDB("clearkillselected", tueur, tuer, dateKill, 0);
				}
				else if (((dateActuel - derniereDate) < tempsMinimum) &&
				 	((dateKill - derniereDate) > 0)  &&
					((dateKill - derniereDate) < tempsMinimum))
				{
					detectionTemp++;
					if (detectionTemp >= detectionMax)
					{
						char szQuery2[1024+1];
						snprintf(szQuery2, sizeof(szQuery2), "SELECT * FROM anti_cheat_detection%s WHERE pseudo = '%s';", get_table_postfix(), tueur->GetName());
						std::unique_ptr<SQLMsg> msgDetectionCheat(DBManager::instance().DirectQuery(szQuery2));
						if (msgDetectionCheat->Get()->uiNumRows > 0)
						{
							MYSQL_ROW row2 = mysql_fetch_row(msgDetectionCheat->Get()->pSQLResult);
							int dateDetection = 0;
							int detectionObtenue = 0;
							int recordBan = 0;
							str_to_number(dateDetection, row2[3]);
							str_to_number(detectionObtenue, row2[2]);
							str_to_number(recordBan, row2[5]);
							if (dateDetection + (tempsClear * 60) < dateActuel)
							{
								UpdateDB("cleardetection", tueur, tuer, dateKill, recordBan);
							}
							UpdateDB("record_detection", tueur, tuer, dateKill, recordBan);
							UpdateDB("detection", tueur, tuer, dateKill, recordBan);
							UpdateDB("date_detection", tueur, tuer, dateKill, recordBan);
							if (detectionObtenue >= detectionMax && detectionObtenue < detectionAvertissement)
							{
								UpdateDB("avertissement", tueur, tuer, dateKill, recordBan);
							}
							if (detectionObtenue >= detectionBan)
							{
								tueur->ChatPacket(CHAT_TYPE_INFO, "Vous avez été banni pour cheat PvP !");
								tueur->ChatPacket(CHAT_TYPE_INFO, "Vous allez être déconnecté !!");
								UpdateDB("ban", tueur, tuer, dateKill, recordBan);
							}
						}
					}
					break;
				}
				derniereDate = dateKill;
			}
		
		}
		else
		{
		}
	}
}

int AntiCheat::GetDateActuel()
{
	return get_global_time();
}

void AntiCheat::Email(LPCHARACTER tueur, const char* titre, const char* texte)
{
	//std::unique_ptr<SQLMsg> pmsgMail (DBManager::instance().DirectQuery("INSERT INTO account_alpha.account_email (compteID, titreEmail, texteEmail, statutEmail) VALUES('%d', '%s', '%s', '%s');", tueur->GetAID(), titre, texte, "NOUVEAU"));
}

void AntiCheat::BanPVP(LPCHARACTER tueur, int recordBan)
{
	Statut = 1;
	if (recordBan < recordBanMax)
	{
		int heureBan = number(1, 15) * 3600;
		std::unique_ptr<SQLMsg> pmsgBan (DBManager::instance().DirectQuery("UPDATE account_alpha.account SET availDt = FROM_UNIXTIME(UNIX_TIMESTAMP(CURRENT_TIMESTAMP()) + %i) WHERE id='%d';", heureBan, tueur->GetAID()));
		std::unique_ptr<SQLMsg> pmsgRecordBan (DBManager::instance().DirectQuery("UPDATE anti_cheat_detection%s SET record_ban = record_ban+1 WHERE pseudo='%s';", get_table_postfix(), tueur->GetName()));
		
		std::string heureBanTexte = ""+heureBan;
		std::string titre = "Vous avez été banni pendant " + heureBanTexte;
		std::string texte = "<p> Chère " + (std::string)tueur->GetName() + "vous avez été banni pendant " + heureBanTexte + " pour Cheat PVP </p><br><p>Veuillez à ce que ça ne ce reproduise plus !</p><br><p>Cordialement, l'Equipe.</p><br>";
		Email(tueur, titre.c_str(), texte.c_str());
 		//tueur->WarpSet(, y);
		tueur->GetDesc()->DelayedDisconnect(number(15, 30));
	}
	else
	{
		const char* titre = "Vous avez été banni pendant définitivement";
		const char* texte = ("<p> Chère %s, vous avez été banni définitivement pour Cheat PVP répété.</p><br><p>Nous en sommes, nous vous souhaitons une bonne continuation.</p><br><p>Cordialement, l'Equipe.</p><br>", tueur->GetName());
		Email(tueur, titre, texte);
		std::unique_ptr<SQLMsg> pmsgBan (DBManager::instance().DirectQuery("UPDATE account_alpha.account SET status = 'BLOCK' WHERE id='%d';", tueur->GetAID()));
		tueur->GetDesc()->DelayedDisconnect(number(15, 30));
	}
}

void AntiCheat::UpdateDB(std::string ordre, LPCHARACTER tueur, LPCHARACTER tuer, int dateAncienKill, int recordBan)
{
	if (ordre == "clearkillselected")
	{
		std::unique_ptr<SQLMsg> pmsgClearSelectedKill (DBManager::instance().DirectQuery("DELETE FROM anti_cheat_pvp%s WHERE tueur='%s' AND datekill='%d';", get_table_postfix(), tueur->GetName(), dateAncienKill));
	}
	if (ordre == "clearkill")
	{
		std::unique_ptr<SQLMsg> pmsgClearAllKill (DBManager::instance().DirectQuery("DELETE FROM anti_cheat_pvp%s WHERE tueur='%s';", get_table_postfix(), tueur->GetName()));
	}
	if (ordre == "cleardetection")
	{
		std::unique_ptr<SQLMsg> pmsgClearAllKill (DBManager::instance().DirectQuery("UPDATE anti_cheat_detection%s SET detection=0 WHERE pseudo='%s';", get_table_postfix(), tueur->GetName()));
	}
	if (ordre == "logkill")
	{
		std::unique_ptr<SQLMsg> pmsgKill (DBManager::instance().DirectQuery("INSERT INTO anti_cheat_pvp%s (tueur, vaincu, dateKill) VALUES('%s', '%s', '%d');", get_table_postfix(), tueur->GetName(), tuer->GetName(), GetDateActuel()));
	}
	if (ordre == "detection")
	{
		std::unique_ptr<SQLMsg> pmsgDetection (DBManager::instance().DirectQuery("UPDATE anti_cheat_detection%s SET detection=detection+1 WHERE pseudo='%s';", get_table_postfix(), tueur->GetName()));
	}
	if (ordre == "record_detection")
	{
		std::unique_ptr<SQLMsg> pmsgRecordDetection (DBManager::instance().DirectQuery("UPDATE anti_cheat_detection%s SET record_detection=record_detection+1 WHERE pseudo='%s';", get_table_postfix(), tueur->GetName()));
	}
	if (ordre == "date_detection")
	{
		std::unique_ptr<SQLMsg> pmsgDateDetection (DBManager::instance().DirectQuery("UPDATE anti_cheat_detection%s SET last_date='%d' WHERE pseudo='%s';", get_table_postfix(), GetDateActuel(), tueur->GetName()));
	}
	if (ordre == "avertissement")
	{
		tueur->ChatPacket(CHAT_TYPE_INFO, "Détection de cheat PvP détecté, vous risquez d'être banni !"); 
		tueur->ChatPacket(CHAT_TYPE_COMMAND, "TitreMessage Détection-de-cheat-PvP-détecté,-vous-risquez-d'être-banni-!");
	}
	if (ordre == "ban")
	{
		tueur->ChatPacket(CHAT_TYPE_COMMAND, "TitreMessage Cheat-PvP-confirmé-!-Vous-êtes-banni-!");
		BanPVP(tueur, recordBan);
	}
}

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
#include "char_environnement.h"
#include "char_meteo.h"
#include "db.h"
#include <sstream>
#include <iostream>
#include <string>
#include <time.h>

using namespace std;

std::vector<std::string> region_environnement_vector; // Liste des noms d'environnement.
std::vector<int> map_index_environnement_vector; // Liste des map index d'environnement.
std::vector<int> coorX1_environnement_vector; // Liste de coordonnée du rayon X 1 environnementale.
std::vector<int> coorY1_environnement_vector; // Liste de coordonnée du rayon Y 1 environnementale.
std::vector<int> coorX1Fin_environnement_vector; // Liste de coordonnée du rayon X1 Fin environnementale.
std::vector<int> coorY1Fin_environnement_vector; // Liste de coordonnée du rayon Y1 Fin environnementale.

std::vector<int> coorX2_environnement_vector; // Liste de coordonnée du rayon X 2 environnementale.
std::vector<int> coorY2_environnement_vector; // Liste de coordonnée du rayon Y 2 environnementale.
std::vector<int> coorX2Fin_environnement_vector; // Liste de coordonnée du rayon X2 Fin environnementale.
std::vector<int> coorY2Fin_environnement_vector; // Liste de coordonnée du rayon Y2 Fin environnementale.

std::vector<int> coorX3_environnement_vector; // Liste de coordonnée du rayon X 3 environnementale.
std::vector<int> coorY3_environnement_vector; // Liste de coordonnée du rayon Y 3 environnementale.
std::vector<int> coorX3Fin_environnement_vector; // Liste de coordonnée du rayon X3 Fin environnementale.
std::vector<int> coorY3Fin_environnement_vector; // Liste de coordonnée du rayon Y3 Fin environnementale.

std::vector<int> coorX4_environnement_vector; // Liste de coordonnée du rayon X 4 environnementale.
std::vector<int> coorY4_environnement_vector; // Liste de coordonnée du rayon Y 4 environnementale.
std::vector<int> coorX4Fin_environnement_vector; // Liste de coordonnée du rayon X4 Fin environnementale.
std::vector<int> coorY4Fin_environnement_vector; // Liste de coordonnée du rayon Y4 Fin environnementale.

std::vector<std::string> ciel_environnement_vector; // Liste des skybox environnementale.
std::vector<std::string> musique_environnement_vector; // Liste des musiques environnementale.

// Evènement de mise à jour automatique du système d'environnement.
EVENTINFO(environnementsysteme_event_info) 
{
	LPCHARACTER ch;
};

// Fonction de mise à jour automatique du système d'environnement.
EVENTFUNC(environnementsysteme_update_event) 
{
	environnementsysteme_event_info* info = dynamic_cast<environnementsysteme_event_info*>( event->info );

	LPCHARACTER ch = info->ch;
	if ( info == NULL )
	{
		sys_err( "check_speedhack_event> <Factor> Null pointer" );
		return 0;
	}
	if (ch == NULL || ch->m_environnementSystem == NULL)
	{
		return 0;
	}
	ch->m_environnementSystem->UpdateEnvironnement(ch); // Lance la mise à jour automatique.
 	return PASSES_PER_SEC(10);
}

bool ChargementEnvironnement()
{
	if(region_environnement_vector.size() > 0)
	{
		region_environnement_vector.clear();
		map_index_environnement_vector.clear();
		coorX1_environnement_vector.clear();
		coorY1_environnement_vector.clear();
		coorX1Fin_environnement_vector.clear();
		coorY1Fin_environnement_vector.clear();
		coorX2_environnement_vector.clear();
		coorY2_environnement_vector.clear();
		coorX2Fin_environnement_vector.clear();
		coorY2Fin_environnement_vector.clear();
		coorX3_environnement_vector.clear();
		coorY3_environnement_vector.clear();
		coorX3Fin_environnement_vector.clear();
		coorY3Fin_environnement_vector.clear();
		coorX4_environnement_vector.clear();
		coorY4_environnement_vector.clear();
		coorX4Fin_environnement_vector.clear();
		coorY4Fin_environnement_vector.clear();
		ciel_environnement_vector.clear();
		musique_environnement_vector.clear();
	}
	std::unique_ptr<SQLMsg> pMsgENVIRONNEMENT(DBManager::instance().DirectQuery("SELECT region, map_index, coorX1, coorY1, coorFinX1, coorFinY1, coorX2, coorY2, coorFinX2, coorFinY2, coorX3, coorY3, coorFinX3, coorFinY3, coorX4, coorY4, coorFinX4, coorFinY4, environnement, musique FROM liste_environnement%s", get_table_postfix()));
	if (pMsgENVIRONNEMENT->uiSQLErrno != 0)
	{
		sys_err("Impossible de charger la liste des informations d'environnement.");
		fprintf(stdout, "Impossible de charger la table d'environnement depuis mysql!\n");
		return false;
	}
	region_environnement_vector.reserve(pMsgENVIRONNEMENT->Get()->uiNumRows);
	map_index_environnement_vector.reserve(pMsgENVIRONNEMENT->Get()->uiNumRows);
	coorX1_environnement_vector.reserve(pMsgENVIRONNEMENT->Get()->uiNumRows);
	coorY1_environnement_vector.reserve(pMsgENVIRONNEMENT->Get()->uiNumRows);
	coorX1Fin_environnement_vector.reserve(pMsgENVIRONNEMENT->Get()->uiNumRows);
	coorY1Fin_environnement_vector.reserve(pMsgENVIRONNEMENT->Get()->uiNumRows);
	coorX2_environnement_vector.reserve(pMsgENVIRONNEMENT->Get()->uiNumRows);
	coorY2_environnement_vector.reserve(pMsgENVIRONNEMENT->Get()->uiNumRows);
	coorX2Fin_environnement_vector.reserve(pMsgENVIRONNEMENT->Get()->uiNumRows);
	coorY2Fin_environnement_vector.reserve(pMsgENVIRONNEMENT->Get()->uiNumRows);
	coorX3_environnement_vector.reserve(pMsgENVIRONNEMENT->Get()->uiNumRows);
	coorY3_environnement_vector.reserve(pMsgENVIRONNEMENT->Get()->uiNumRows);
	coorX3Fin_environnement_vector.reserve(pMsgENVIRONNEMENT->Get()->uiNumRows);
	coorY3Fin_environnement_vector.reserve(pMsgENVIRONNEMENT->Get()->uiNumRows);
	coorX4_environnement_vector.reserve(pMsgENVIRONNEMENT->Get()->uiNumRows);
	coorY4_environnement_vector.reserve(pMsgENVIRONNEMENT->Get()->uiNumRows);
	coorX4Fin_environnement_vector.reserve(pMsgENVIRONNEMENT->Get()->uiNumRows);
	coorY4Fin_environnement_vector.reserve(pMsgENVIRONNEMENT->Get()->uiNumRows);
	ciel_environnement_vector.reserve(pMsgENVIRONNEMENT->Get()->uiNumRows);
	musique_environnement_vector.reserve(pMsgENVIRONNEMENT->Get()->uiNumRows);

	MYSQL_ROW rowENVIRONNEMENT;
	int tempmapindex = 0;
	int coorx1 = 0;
	int coory1 = 0;
	int coorx1fin = 0;
	int coory1fin = 0;
	int coorx2 = 0;
	int coory2 = 0;
	int coorx2fin = 0;
	int coory2fin = 0;
	int coorx3 = 0;
	int coory3 = 0;
	int coorx3fin = 0;
	int coory3fin = 0;
	int coorx4 = 0;
	int coory4 = 0;
	int coorx4fin = 0;
	int coory4fin = 0;
	
	while((rowENVIRONNEMENT = mysql_fetch_row(pMsgENVIRONNEMENT->Get()->pSQLResult)))
	{
		region_environnement_vector.emplace_back(rowENVIRONNEMENT[0]);
		str_to_number(tempmapindex, rowENVIRONNEMENT[1]);
		str_to_number(coorx1, rowENVIRONNEMENT[2]);
		str_to_number(coory1, rowENVIRONNEMENT[3]);
		str_to_number(coorx1fin, rowENVIRONNEMENT[4]);
		str_to_number(coory1fin, rowENVIRONNEMENT[5]);
		str_to_number(coorx2, rowENVIRONNEMENT[6]);
		str_to_number(coory2, rowENVIRONNEMENT[7]);
		str_to_number(coorx2fin, rowENVIRONNEMENT[8]);
		str_to_number(coory2fin, rowENVIRONNEMENT[9]);
		str_to_number(coorx3, rowENVIRONNEMENT[10]);
		str_to_number(coory3, rowENVIRONNEMENT[11]);
		str_to_number(coorx3fin, rowENVIRONNEMENT[12]);
		str_to_number(coory3fin, rowENVIRONNEMENT[13]);

		str_to_number(coorx4, rowENVIRONNEMENT[14]);
		str_to_number(coory4, rowENVIRONNEMENT[15]);
		str_to_number(coorx4fin, rowENVIRONNEMENT[16]);
		str_to_number(coory4fin, rowENVIRONNEMENT[17]);

		map_index_environnement_vector.emplace_back(tempmapindex);
		coorX1_environnement_vector.emplace_back(coorx1);
		coorY1_environnement_vector.emplace_back(coory1);
		coorX1Fin_environnement_vector.emplace_back(coorx1fin);
		coorY1Fin_environnement_vector.emplace_back(coory1fin);

		coorX2_environnement_vector.emplace_back(coorx2);
		coorY2_environnement_vector.emplace_back(coory2);
		coorX2Fin_environnement_vector.emplace_back(coorx2fin);
		coorY2Fin_environnement_vector.emplace_back(coory2fin);

		coorX3_environnement_vector.emplace_back(coorx3);
		coorY3_environnement_vector.emplace_back(coory3);
		coorX3Fin_environnement_vector.emplace_back(coorx3fin);
		coorY3Fin_environnement_vector.emplace_back(coory3fin);

		coorX4_environnement_vector.emplace_back(coorx4);
		coorY4_environnement_vector.emplace_back(coory4);
		coorX4Fin_environnement_vector.emplace_back(coorx4fin);
		coorY4Fin_environnement_vector.emplace_back(coory4fin);

		ciel_environnement_vector.emplace_back(rowENVIRONNEMENT[18]);
		musique_environnement_vector.emplace_back(rowENVIRONNEMENT[19]);
	}
	return true;	
}

void Environnement::InitialisationEnvironnement(LPCHARACTER ch)
{
	if (m_pkEnvironnementSystemeUpdateEvent == NULL)
	{
		environnementsysteme_event_info* info = AllocEventInfo<environnementsysteme_event_info>();
		info->ch = ch;
		m_pkEnvironnementSystemeUpdateEvent = event_create(environnementsysteme_update_event, info, PASSES_PER_SEC(10));
	}
}

void Environnement::UpdateEnvironnement(LPCHARACTER ch)
{
	if (ch->GetDungeon() == false)
	{
		int joueurMapIndex = ch->GetMapIndex();
		int joueurCoorX = ch->GetX();
		int joueurCoorY = ch->GetY();

		int regionMoyenneX1 = 0;
		int regionMoyenneY1 = 0;
		int regionMoyenneX2 = 0;
		int regionMoyenneY2 = 0;
		int regionMoyenneX3 = 0;
		int regionMoyenneY3 = 0;
		int regionMoyenneX4 = 0;
		int regionMoyenneY4 = 0;
		//ch->ChatPacket(CHAT_TYPE_INFO, "Environnement automatique C++: Map Index: %d, coor X: %d, coor Y: %d", joueurMapIndex, joueurCoorX, joueurCoorY);
		for (int i = 0; i < region_environnement_vector.size(); i++)
		{
			if (regionActuel != region_environnement_vector[i])
			{
				if (joueurMapIndex == map_index_environnement_vector[i])
				{
					bool EnvironnementActivation = false;
					regionMoyenneX1 = coorX1_environnement_vector[i] + (coorX1Fin_environnement_vector[i] / coorY1_environnement_vector[i]);
					regionMoyenneY1 = coorY1_environnement_vector[i] + (coorY1Fin_environnement_vector[i] / coorX1_environnement_vector[i]);
					regionMoyenneX2 = coorX1Fin_environnement_vector[i] + (coorX1_environnement_vector[i] / coorY1_environnement_vector[i]);
					regionMoyenneY2 = coorY1Fin_environnement_vector[i] + (coorY1_environnement_vector[i] / coorX1_environnement_vector[i]);

					regionMoyenneX3 = coorX2_environnement_vector[i] + (coorX2Fin_environnement_vector[i] / coorY2_environnement_vector[i]);
					regionMoyenneY3 = coorY2_environnement_vector[i] + (coorY2Fin_environnement_vector[i] / coorX2_environnement_vector[i]);

					regionMoyenneX4 = coorX2Fin_environnement_vector[i] + (coorX2_environnement_vector[i] / coorY2_environnement_vector[i]);
					regionMoyenneY4 = coorY2Fin_environnement_vector[i] + (coorY2_environnement_vector[i] / coorX2_environnement_vector[i]);
				
					if ((joueurCoorX >= coorX3_environnement_vector[i]) && (joueurCoorX <= regionMoyenneX1))
					{
						if ((joueurCoorY >= regionMoyenneY1) && (joueurCoorY <= coorY3_environnement_vector[i]))
						{
							EnvironnementActivation = true;
						}
					}
					if ((joueurCoorX >= regionMoyenneX1) && (joueurCoorX <= regionMoyenneX4))
					{
						if ((joueurCoorY >= regionMoyenneY1) && (joueurCoorY <= regionMoyenneY4))
						{
							EnvironnementActivation = true;
						}
					}
					if ((joueurCoorX >= regionMoyenneX2) && (joueurCoorX <= regionMoyenneX3))
					{
						if ((joueurCoorY >= regionMoyenneY2) && (joueurCoorY <= regionMoyenneY3))
						{
							EnvironnementActivation = true;
						}
					}
					if ((joueurCoorX >= regionMoyenneX4) && (joueurCoorX <= regionMoyenneX1))
					{
						if ((joueurCoorY >= regionMoyenneY4) && (joueurCoorY <= regionMoyenneY1))
						{
							EnvironnementActivation = true;
						}
					}
					if (EnvironnementActivation)
					{
						if (ciel_environnement_vector[i] != "" && ciel_environnement_vector[i] != "defaut")
						{
							ch->ChatPacket(CHAT_TYPE_COMMAND, "environnement %s", ciel_environnement_vector[i].c_str());
						}
						if (musique_environnement_vector[i] != "" && musique_environnement_vector[i] != "defaut")
						{
							ch->ChatPacket(CHAT_TYPE_COMMAND, "musique %s", musique_environnement_vector[i].c_str());
						}
						if (region_environnement_vector[i] != "" && region_environnement_vector[i] != "defaut")
						{
							ch->ChatPacket(CHAT_TYPE_COMMAND, "region %s", region_environnement_vector[i].c_str());
						}
						regionActuel = region_environnement_vector[i];
					}
				}
			}
			else
			{
				if (joueurMapIndex == map_index_environnement_vector[i])
				{
					int compteurNonRegion = 0;
					regionMoyenneX1 = coorX1_environnement_vector[i] + (coorX1Fin_environnement_vector[i] / coorY1_environnement_vector[i]);
					regionMoyenneY1 = coorY1_environnement_vector[i] + (coorY1Fin_environnement_vector[i] / coorX1_environnement_vector[i]);
					regionMoyenneX2 = coorX1Fin_environnement_vector[i] + (coorX1_environnement_vector[i] / coorY1_environnement_vector[i]);
					regionMoyenneY2 = coorY1Fin_environnement_vector[i] + (coorY1_environnement_vector[i] / coorX1_environnement_vector[i]);

					regionMoyenneX3 = coorX2_environnement_vector[i] + (coorX2Fin_environnement_vector[i] / coorY2_environnement_vector[i]);
					regionMoyenneY3 = coorY2_environnement_vector[i] + (coorY2Fin_environnement_vector[i] / coorX2_environnement_vector[i]);

					regionMoyenneX4 = coorX2Fin_environnement_vector[i] + (coorX2_environnement_vector[i] / coorY2_environnement_vector[i]);
					regionMoyenneY4 = coorY2Fin_environnement_vector[i] + (coorY2_environnement_vector[i] / coorX2_environnement_vector[i]);

					if ((joueurCoorX >= coorX3_environnement_vector[i]) && (joueurCoorX <= regionMoyenneX1))
					{
						compteurNonRegion--;
						if ((joueurCoorY >= regionMoyenneY1) && (joueurCoorY <= coorY3_environnement_vector[i]))
						{
							compteurNonRegion--;
						}
						else
						{
							compteurNonRegion++;
						}
					}
					else
					{
						compteurNonRegion++;
					}
					if ((joueurCoorX >= regionMoyenneX1) && (joueurCoorX <= regionMoyenneX4))
					{
						compteurNonRegion--;
						if ((joueurCoorY >= regionMoyenneY1) && (joueurCoorY <= regionMoyenneY4))
						{
							compteurNonRegion--;
						}
						else
						{
							compteurNonRegion++;
						}
					}
					else
					{
						compteurNonRegion++;
					}
					if ((joueurCoorX >= regionMoyenneX2) && (joueurCoorX <= regionMoyenneX3))
					{
						compteurNonRegion--;
						if ((joueurCoorY >= regionMoyenneY2) && (joueurCoorY <= regionMoyenneY3))
						{
							compteurNonRegion--;
						}
						else
						{
							compteurNonRegion++;
						}
					}
					else
					{
						compteurNonRegion++;
					}
					if ((joueurCoorX >= regionMoyenneX4) && (joueurCoorX <= regionMoyenneX1))
					{
						compteurNonRegion--;
						if ((joueurCoorY >= regionMoyenneY4) && (joueurCoorY <= regionMoyenneY1))
						{
							compteurNonRegion--;
						}
						else
						{
							compteurNonRegion++;
						}
					}
					else
					{
						compteurNonRegion++;
					}
					if (compteurNonRegion >= 2) // Remet la météo normal si le joueur ne fait plus partit de la région où il été.
					{
						regionActuel = "";
						if (ch->m_meteoSystem != NULL)
						{
							ch->m_meteoSystem->MSJMeteo = false; // Initialisation à 0 de la météo actuel pour la mettre à jour.
						}
					}
				}
			}
		}
	}
}

				
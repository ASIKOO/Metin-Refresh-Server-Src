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
#include "char_titre.h"
#include "db.h"
#include "entity.h"
#include <sstream>
#include <iostream>
#include <string>

using namespace std;


std::vector<int> cache_titre_vector;
std::vector<std::string> point_vector;
std::vector<std::string> titre_vector;
std::vector<std::string> titre_femme_vector;
std::vector<std::string> couleur1_vector;
std::vector<std::string> couleur2_vector;
std::vector<std::string> couleur3_vector;
std::vector<std::string> description_vector;
std::vector<std::string> description_lock_vector;
std::vector<int> typebonus1_vector;
std::vector<int> valeurbonus1_vector;
std::vector<int> typebonus2_vector;
std::vector<int> valeurbonus2_vector;
std::vector<int> typebonus3_vector;
std::vector<int> valeurbonus3_vector;
std::vector<std::string> titre_couleur_vector;
std::vector<std::string> titre_effet_vector;
std::vector<int> objectif_titre_vector;


void TitreSysteme::ChargementTitre(LPCHARACTER ch)
{
	if (titre_vector_joueur.size() > 0)
	{
		titre_vector_joueur.clear();
	}
	std::unique_ptr<SQLMsg> pMsgTITRE(DBManager::instance().DirectQuery("SELECT titre, active FROM player_titre%s WHERE pseudo='%s'", get_table_postfix(), ch->GetName()));
	titre_vector_joueur.reserve(pMsgTITRE->Get()->uiNumRows);
	MYSQL_ROW rowTITRE;
	int active = 0;
	if (pMsgTITRE->Get()->uiNumRows > 0)
	{
		while((rowTITRE = mysql_fetch_row(pMsgTITRE->Get()->pSQLResult)))
    		{
			std::string tempTitre = rowTITRE[0];
			replace(tempTitre.begin(), tempTitre.end(), ' ', '-');
			titre_vector_joueur.emplace_back(tempTitre);
			str_to_number(active, rowTITRE[1]);
			if (active == 1)
			{
				if (ch->GetRaceNum() == 0 || ch->GetRaceNum() == 2 || ch->GetRaceNum() == 5 || ch->GetRaceNum() == 7 || ch->GetRaceNum() == 8) // Homme
				{
					for (int i = 0; i < titre_vector.size(); i++)
					{
						if (rowTITRE[0] == titre_vector[i].c_str())
						{
							ch->m_titreSystem->niveauTitreTueur = i+1;
						}
					}
				}
				else
				{
					for (int i = 0; i < titre_femme_vector.size(); i++)
					{
						if (rowTITRE[0] == titre_femme_vector[i].c_str())
						{
							ch->m_titreSystem->niveauTitreTueur = i+1;
						}
					}
				}
			}	
		}
	}
	if (id_titre_objectif_vector_joueur.size() > 0)
	{
		id_titre_objectif_vector_joueur.clear();
		pourcentage_titre_objectif_vector_joueur.clear();
	}
	std::unique_ptr<SQLMsg> pMsgTITREOBJECTIF(DBManager::instance().DirectQuery("SELECT id_titre, pourcentage FROM player_titre_objectif%s WHERE idJoueur='%d'", get_table_postfix(), ch->GetPlayerID()));
	id_titre_objectif_vector_joueur.reserve(pMsgTITREOBJECTIF->Get()->uiNumRows);
	pourcentage_titre_objectif_vector_joueur.reserve(pMsgTITREOBJECTIF->Get()->uiNumRows);
	MYSQL_ROW rowTITREOBJECTIF;
	if (pMsgTITREOBJECTIF->Get()->uiNumRows > 0)
	{
		int cacheObjectif = 0;
		while((rowTITREOBJECTIF = mysql_fetch_row(pMsgTITREOBJECTIF->Get()->pSQLResult)))
    		{
			str_to_number(cacheObjectif, rowTITREOBJECTIF[0]);
			id_titre_objectif_vector_joueur.emplace_back(cacheObjectif);
			str_to_number(cacheObjectif, rowTITREOBJECTIF[1]);
			pourcentage_titre_objectif_vector_joueur.emplace_back(cacheObjectif);
		}
	}
}


// Charge la liste des titres, couleur, description si nécessaire.
bool ChargementListeTitreMYSQL()
{
	if (titre_vector.size() > 0) // Nettoie la liste en cas de mise à jour.
	{
		cache_titre_vector.clear();
		titre_vector.clear();
		titre_femme_vector.clear();
		point_vector.clear();
		couleur1_vector.clear();
		couleur2_vector.clear();
		couleur3_vector.clear();
		description_vector.clear();
		typebonus1_vector.clear();
		valeurbonus1_vector.clear();
		typebonus2_vector.clear();
		valeurbonus2_vector.clear();
		typebonus3_vector.clear();
		valeurbonus3_vector.clear();
		titre_couleur_vector.clear();
		titre_effet_vector.clear();
		description_lock_vector.clear();
		objectif_titre_vector.clear();
	}

	std::unique_ptr<SQLMsg> pMsgTITRE(DBManager::instance().DirectQuery("SELECT id, titre, titrefemme, point, color1, color2, color3, idbonus1, valeurbonus1, idbonus2, valeurbonus2, idbonus3, valeurbonus3, titre_couleur, titre_effet, objectif FROM player_liste_titre%s", get_table_postfix()));
	std::unique_ptr<SQLMsg> pMsgDESC(DBManager::instance().DirectQuery("SELECT description, descriptionLock FROM player_liste_titre%s", get_table_postfix()));

	if (pMsgTITRE->uiSQLErrno != 0)
	{
		sys_err("Impossible de charger la liste des informations de titre.");
		fprintf(stdout, "Impossible de charger la table des titres depuis mysql!\n");
		return false;
	}
	cache_titre_vector.reserve(pMsgTITRE->Get()->uiNumRows);
	titre_vector.reserve(pMsgTITRE->Get()->uiNumRows);
	point_vector.reserve(pMsgTITRE->Get()->uiNumRows);
	couleur1_vector.reserve(pMsgTITRE->Get()->uiNumRows);
	couleur2_vector.reserve(pMsgTITRE->Get()->uiNumRows);
	couleur3_vector.reserve(pMsgTITRE->Get()->uiNumRows);
	typebonus1_vector.reserve(pMsgTITRE->Get()->uiNumRows);
	valeurbonus1_vector.reserve(pMsgTITRE->Get()->uiNumRows);
	typebonus2_vector.reserve(pMsgTITRE->Get()->uiNumRows);
	valeurbonus2_vector.reserve(pMsgTITRE->Get()->uiNumRows);
	typebonus3_vector.reserve(pMsgTITRE->Get()->uiNumRows);
	valeurbonus3_vector.reserve(pMsgTITRE->Get()->uiNumRows);

	description_vector.reserve(pMsgDESC->Get()->uiNumRows);
	description_lock_vector.reserve(pMsgDESC->Get()->uiNumRows);
	titre_couleur_vector.reserve(pMsgTITRE->Get()->uiNumRows);
	titre_effet_vector.reserve(pMsgTITRE->Get()->uiNumRows);
	titre_femme_vector.reserve(pMsgTITRE->Get()->uiNumRows);
	objectif_titre_vector.reserve(pMsgTITRE->Get()->uiNumRows);

	MYSQL_ROW rowTITRE;
	int compteur = 0;
	int cacheTitre;
	while((rowTITRE = mysql_fetch_row(pMsgTITRE->Get()->pSQLResult)))
    	{
		str_to_number(cacheTitre, rowTITRE[0]);
		cache_titre_vector.emplace_back(cacheTitre);

		std::string tempTitre = rowTITRE[1];
		replace(tempTitre.begin(), tempTitre.end(), ' ', '-');
		titre_vector.emplace_back(tempTitre);
		std::string tempTitreFemme = rowTITRE[2];
		replace(tempTitreFemme.begin(), tempTitreFemme.end(), ' ', '-');
		sys_err("titre femme %s", tempTitreFemme.c_str());
		titre_femme_vector.emplace_back(tempTitreFemme.c_str());
		point_vector.emplace_back(rowTITRE[3]);
		couleur1_vector.emplace_back(rowTITRE[4]);
		couleur2_vector.emplace_back(rowTITRE[5]);
		couleur3_vector.emplace_back(rowTITRE[6]);

		str_to_number(cacheTitre, rowTITRE[7]);
		typebonus1_vector.emplace_back(cacheTitre);
		str_to_number(cacheTitre, rowTITRE[8]);
		valeurbonus1_vector.emplace_back(cacheTitre);

		str_to_number(cacheTitre, rowTITRE[9]);
		typebonus2_vector.emplace_back(cacheTitre);
		str_to_number(cacheTitre, rowTITRE[10]);
		valeurbonus2_vector.emplace_back(cacheTitre);

		str_to_number(cacheTitre, rowTITRE[11]);
		typebonus3_vector.emplace_back(cacheTitre);
		str_to_number(cacheTitre, rowTITRE[12]);
		valeurbonus3_vector.emplace_back(cacheTitre);
		titre_couleur_vector.emplace_back(rowTITRE[13]);
		titre_effet_vector.emplace_back(rowTITRE[14]);
		str_to_number(cacheTitre, rowTITRE[15]);
		objectif_titre_vector.emplace_back(cacheTitre);

	}

	MYSQL_ROW rowDESC;
	while((rowDESC = mysql_fetch_row(pMsgDESC->Get()->pSQLResult)))
    	{
		description_vector.emplace_back(rowDESC[0]);
		description_lock_vector.emplace_back(rowDESC[1]);
	}

	return true;
}




// Evènement de mise à jour automatique du système de titre.
EVENTINFO(titresysteme_event_info) 
{
	LPCHARACTER pkChr;
};

// Fonction de mise à jour automatique du système de titre.
EVENTFUNC(titresysteme_update_event) 
{
	titresysteme_event_info* info = dynamic_cast<titresysteme_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "check_speedhack_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER pkChr = info->pkChr;
	if (pkChr == NULL)
	{
		return 0;
	}
	if(pkChr->last_X != pkChr->GetX() || pkChr->last_Y != pkChr->GetY())
	{
		pkChr->last_X = pkChr->GetX();
		pkChr->last_Y = pkChr->GetY();
		pkChr->m_titreSystem->compteurAfk = 0;
		if (pkChr->m_titreSystem->AFK == true)
		{
			pkChr->m_titreSystem->AFK = false;
			pkChr->UpdatePacket();
		}
	}
	else
	{
		pkChr->m_titreSystem->compteurAfk++;
		pkChr->UpdatePacket();
	}
	if(pkChr->m_titreSystem->compteurAfk >= 10)
	{
		pkChr->m_titreSystem->AFK = true;
		pkChr->UpdatePacket();
	}
	else
	{
		pkChr->m_titreSystem->AFK = false;
	}

	/*if (pkChr->m_titreSystem != NULL)
	{
		pkChr->SetTitle(pkChr->m_titreSystem->niveauTitreTueur);
		pkChr->PointChange(POINT_TITLE, pkChr->m_titreSystem->niveauTitreTueur);
		pkChr->UpdatePacket();
		
	}*/
 	return PASSES_PER_SEC(60);
}


// Evènement de mise à jour automatique du système de titre.
EVENTINFO(titresysteme_objectif_event_info) 
{
	LPCHARACTER pkChr;
};

// Fonction de mise à jour automatique du système de titre.
EVENTFUNC(titresysteme_objectif_update_event) 
{
	titresysteme_objectif_event_info* info = dynamic_cast<titresysteme_objectif_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "check_speedhack_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER pkChr = info->pkChr;

	if (pkChr == NULL)
	{
		return 0;
	}

	for (int i = 0; i < titre_vector.size(); i++)
	{
		for (int b = 0; b < pkChr->m_titreSystem->id_titre_objectif_vector_joueur.size(); b++)
		{
			if (pkChr->m_titreSystem->id_titre_objectif_vector_joueur[b] == cache_titre_vector[i])
			{
				pkChr->ChatPacket(CHAT_TYPE_COMMAND, "UpdateTitreProgression %d-%d", b, pkChr->m_titreSystem->pourcentage_titre_objectif_vector_joueur[b]);
			}
		}
	}
	const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();

	for (itertype(c_ref_set) it = c_ref_set.begin(); it != c_ref_set.end(); ++it)
	{
		LPCHARACTER chUpdate = (*it)->GetCharacter();

		if (!chUpdate)
			continue;

		if (chUpdate->IsPC())
		{
			pkChr->m_titreSystem->GetPacketTitre(pkChr, pkChr->GetName(), pkChr->m_titreSystem->niveauTitreTueur);

			std::string tempTitre = pkChr->m_titreSystem->nomTitre.c_str();
			const char* couleur1 = pkChr->m_titreSystem->couleur1Titre.c_str();
			const char* couleur2 = pkChr->m_titreSystem->couleur2Titre.c_str();
			const char* couleur3 = pkChr->m_titreSystem->couleur3Titre.c_str();

			replace(tempTitre.begin(), tempTitre.end(), ' ', '|');
			if (pkChr->m_titreSystem->AFK == true)
			{
				tempTitre = "AFK";
			}
			chUpdate->ChatPacket(CHAT_TYPE_COMMAND, "TitreFix %s %s %s %s %s", pkChr->GetName(), tempTitre.c_str(), couleur1, couleur2, couleur3);
			pkChr->ChatPacket(CHAT_TYPE_COMMAND, "TitreFix %s %s %s %s %s", pkChr->GetName(), tempTitre.c_str(), couleur1, couleur2, couleur3);

		}
	}

 	return PASSES_PER_SEC(1);
}


EVENTFUNC(titresysteme_updatebonus_event) 
{
	titresysteme_event_info* info = dynamic_cast<titresysteme_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "check_speedhack_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER pkChr = info->pkChr;
	if (pkChr == NULL) 
	{
		return 0;
	}
 	return PASSES_PER_SEC(1) / 4;
}

// Convertit un string en float.
float stof(const std::string& s) {
    std::istringstream str(s);
    float i;
    str >> i;
    return i;
}

void TitreSysteme::UpdateBonusTitre(LPCHARACTER ch, int idTitre)
{
	if (typebonus1_vector[idTitre] > 0)
	{
		ch->AddAffect(AFFECT_COLLECT, aApplyInfo[typebonus1_vector[idTitre]].bPointType, valeurbonus1_vector[idTitre], 0, 60*60*24*365*60, 0, false);
	}
	if (typebonus2_vector[idTitre] > 0)
	{
		ch->AddAffect(AFFECT_COLLECT, aApplyInfo[typebonus2_vector[idTitre]].bPointType, valeurbonus2_vector[idTitre], 0, 60*60*24*365*60, 0, false);
	}
	if (typebonus3_vector[idTitre] > 0)
	{
		ch->AddAffect(AFFECT_COLLECT, aApplyInfo[typebonus3_vector[idTitre]].bPointType, valeurbonus3_vector[idTitre], 0, 60*60*24*365*60, 0, false);
	}
}

void TitreSysteme::RetraitBonusTitre(LPCHARACTER ch)
{
	// Bonus 1
	const std::list<CAffect*>& rList = ch->GetAffectContainer();
	const CAffect* pAffect1 = NULL;
	const CAffect* pAffect2 = NULL;
	const CAffect* pAffect3 = NULL;

	if (typebonus1_vector[niveauTitreTueur] > 0)
	{
		for ( std::list<CAffect*>::const_iterator iter = rList.begin(); iter != rList.end(); ++iter )
		{
			pAffect1 = *iter;

			if ( pAffect1->dwType == AFFECT_COLLECT )
			{
				if ( pAffect1->bApplyOn == aApplyInfo[typebonus1_vector[niveauTitreTueur]].bPointType && pAffect1->lApplyValue == valeurbonus1_vector[niveauTitreTueur] )
				{
					break;
				}
			}
	
			pAffect1 = NULL;
		}

		if ( pAffect1 != NULL )
		{
			ch->RemoveAffect( const_cast<CAffect*>(pAffect1) );
		}
	}

	if (typebonus2_vector[niveauTitreTueur] > 0)
	{
		for ( std::list<CAffect*>::const_iterator iter = rList.begin(); iter != rList.end(); ++iter )
		{
			pAffect2 = *iter;

			if ( pAffect2->dwType == AFFECT_COLLECT )
			{
				if ( pAffect2->bApplyOn == aApplyInfo[typebonus2_vector[niveauTitreTueur]].bPointType && pAffect2->lApplyValue == valeurbonus2_vector[niveauTitreTueur] )
				{
					break;
				}
			}

			pAffect2 = NULL;
		}

		if ( pAffect2 != NULL )
		{
			ch->RemoveAffect( const_cast<CAffect*>(pAffect2) );
		}
	}

	if (typebonus3_vector[niveauTitreTueur] > 0)
	{
		for ( std::list<CAffect*>::const_iterator iter = rList.begin(); iter != rList.end(); ++iter )
		{
			pAffect3 = *iter;

			if ( pAffect3->dwType == AFFECT_COLLECT )
			{
				if ( pAffect3->bApplyOn == aApplyInfo[typebonus3_vector[niveauTitreTueur]].bPointType && pAffect3->lApplyValue == valeurbonus3_vector[niveauTitreTueur] )
				{
					break;
				}
			}

			pAffect3 = NULL;
		}
	

		if ( pAffect3 != NULL )
		{
			ch->RemoveAffect( const_cast<CAffect*>(pAffect3) );
		}
	}
}

// Met à jour en temps réel le système de titre du joueur.
void TitreSysteme::UpdatePacketTitre(DWORD temps, LPCHARACTER ch)
{
	joueur = ch;
	if (m_pkTitreSystemeUpdateEvent == NULL)
	{
		titresysteme_event_info* info = AllocEventInfo<titresysteme_event_info>();

		info->pkChr = ch;

		m_pkTitreSystemeUpdateEvent = event_create(titresysteme_update_event, info, PASSES_PER_SEC(60));
		joueur->m_titreSystem->RetraitBonusTitre(ch);
		joueur->m_titreSystem->UpdateBonusTitre(ch, ch->m_titreSystem->niveauTitreTueur);
	}
	if (m_pkTitreSystemeObjectifUpdateEvent == NULL)
	{
		titresysteme_objectif_event_info* info = AllocEventInfo<titresysteme_objectif_event_info>();

		info->pkChr = ch;

		m_pkTitreSystemeObjectifUpdateEvent = event_create(titresysteme_objectif_update_event, info, PASSES_PER_SEC(1));
	}	
}

void TitreSysteme::ArretUpdateTitre()
{
	if (&m_pkTitreSystemeUpdateEvent != NULL)
	{
		event_cancel(&m_pkTitreSystemeUpdateEvent);
		m_pkTitreSystemeUpdateEvent = NULL;
	}
	if (&m_pkTitreSystemeBonusUpdateEvent != NULL)
	{
		event_cancel(&m_pkTitreSystemeBonusUpdateEvent);
		m_pkTitreSystemeBonusUpdateEvent = NULL;
	}
	if (&m_pkTitreSystemeObjectifUpdateEvent != NULL)
	{
		event_cancel(&m_pkTitreSystemeObjectifUpdateEvent);
		m_pkTitreSystemeObjectifUpdateEvent;
	}
}

// Effectue la mise à jour des packet titres à la demande.
void TitreSysteme::GetPacketTitre(LPCHARACTER ch, const char* nom, int niveauTitre)
{
	joueur = ch;
	std::string tempTitre = "";
	if (niveauTitre == 0 && joueur->GetTitle() == 0 && niveauTitreTueur == 0)
	{
		joueur->SetTitle(1); 
		joueur->PointChange(POINT_TITLE, 1);
		

		if (ch->GetRaceNum() == 0 || ch->GetRaceNum() == 2 || ch->GetRaceNum() == 5 || ch->GetRaceNum() == 7 || ch->GetRaceNum() == 8) // Homme
		{
			tempTitre = titre_vector[1];
		}
		if(ch->GetRaceNum() == 1 || ch->GetRaceNum() == 3 || ch->GetRaceNum() == 4 || ch->GetRaceNum() == 6) // Femme
		{
			tempTitre = titre_femme_vector[1];
		}
		replace(tempTitre.begin(), tempTitre.end(), '-', ' ');
		replace(tempTitre.begin(), tempTitre.end(), '#', '\'');
		nomTitre = tempTitre.c_str();
		couleur1Titre = couleur1_vector[1].c_str(); 
		couleur2Titre = couleur2_vector[1].c_str(); 
		couleur3Titre = couleur3_vector[1].c_str();
	}
	else
	{
		if (niveauTitreTueur == 0)
		{
			niveauTitreTueur = joueur->GetTitle();
			if (ch->GetRaceNum() == 0 || ch->GetRaceNum() == 2 || ch->GetRaceNum() == 5 || ch->GetRaceNum() == 7 || ch->GetRaceNum() == 8) // Homme
			{
				tempTitre = titre_vector[niveauTitreTueur];
			}
			if(ch->GetRaceNum() == 1 || ch->GetRaceNum() == 3 || ch->GetRaceNum() == 4 || ch->GetRaceNum() == 6) // Femme
			{
				tempTitre = titre_femme_vector[niveauTitreTueur];
			}
			replace(tempTitre.begin(), tempTitre.end(), '-', ' ');
			replace(tempTitre.begin(), tempTitre.end(), '#', '\'');
			nomTitre = tempTitre.c_str();
			couleur1Titre = couleur1_vector[niveauTitreTueur].c_str(); 
			couleur2Titre = couleur2_vector[niveauTitreTueur].c_str(); 
			couleur3Titre = couleur3_vector[niveauTitreTueur].c_str();
		}
		else
		{
			if (ch->GetRaceNum() == 0 || ch->GetRaceNum() == 2 || ch->GetRaceNum() == 5 || ch->GetRaceNum() == 7 || ch->GetRaceNum() == 8) // Homme
			{
				tempTitre = titre_vector[niveauTitreTueur];
			}
			if(ch->GetRaceNum() == 1 || ch->GetRaceNum() == 3 || ch->GetRaceNum() == 4 || ch->GetRaceNum() == 6) // Femme
			{
				tempTitre = titre_femme_vector[niveauTitreTueur];
			}
			replace(tempTitre.begin(), tempTitre.end(), '-', ' ');
			replace(tempTitre.begin(), tempTitre.end(), '#', '\'');
			nomTitre = tempTitre.c_str();
			couleur1Titre = couleur1_vector[niveauTitreTueur].c_str(); 
			couleur2Titre = couleur2_vector[niveauTitreTueur].c_str(); 
			couleur3Titre = couleur3_vector[niveauTitreTueur].c_str();
		}
	}
	if (test_server > 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "DEBUG Titre: Utilisation de la liste des titres mise en mémoire.");
	}
	if (titre_vector.size() <= 0 || titre_femme_vector.size() <= 0)
	{
		ChargementListeTitreMYSQL();
	}
}


// Retourne le nom du titre posséder par le joueur.
const char* TitreSysteme::GetNomTitre(LPCHARACTER ch, int niveauTitre)
{
	std::string nomTitre = "";
	if (ch->GetRaceNum() == 0 || ch->GetRaceNum() == 2 || ch->GetRaceNum() == 5 || ch->GetRaceNum() == 7 || ch->GetRaceNum() == 8) // Homme
	{
		nomTitre = titre_vector[niveauTitre];
	}
	if(ch->GetRaceNum() == 1 || ch->GetRaceNum() == 3 || ch->GetRaceNum() == 4 || ch->GetRaceNum() == 6) // Femme
	{
		nomTitre = titre_femme_vector[niveauTitre];
	}
	return nomTitre.c_str();
}

// Supprime un titre du joueur.
void TitreSysteme::DeleteTitre(const char* nomJoueur, const char* titrepvp, const char* nouveautitre)
{
	std::unique_ptr<SQLMsg> pmsg (DBManager::instance().DirectQuery("DELETE FROM player_titre%s WHERE pseudo='%s' and titre='%s';", get_table_postfix(), nomJoueur, titrepvp));
	std::unique_ptr<SQLMsg> pmsgLastTitre (DBManager::instance().DirectQuery("UPDATE player_titre%s SET active=0 WHERE pseudo='%s'", get_table_postfix(), nomJoueur));
	std::unique_ptr<SQLMsg> pmsgLastTitre2 (DBManager::instance().DirectQuery("UPDATE player_titre%s SET active=1 WHERE pseudo='%s' and titre='%s'", get_table_postfix(), nomJoueur, nouveautitre));
}

// Sauvegarde un titre du joueur.
void TitreSysteme::SaveTitre(const char* nomJoueur, const char* titrepvp)
{
	if (VerifTitre(titrepvp, nomJoueur) == false)
	{
		std::string tempTitre = titrepvp;
		char tempSqlTitre[64+1];
		DBManager::instance().EscapeString(tempSqlTitre, sizeof(tempSqlTitre), tempTitre.c_str(), tempTitre.length());
		std::unique_ptr<SQLMsg> pmsg (DBManager::instance().DirectQuery("INSERT INTO player_titre%s (pseudo, titre) VALUES('%s', '%s');", get_table_postfix(), nomJoueur, tempSqlTitre));
		std::unique_ptr<SQLMsg> pmsgLastTitre (DBManager::instance().DirectQuery("UPDATE player_titre%s SET active=0 WHERE pseudo='%s'", get_table_postfix(), nomJoueur));
		std::unique_ptr<SQLMsg> pmsgLastTitre2 (DBManager::instance().DirectQuery("UPDATE player_titre%s SET active=1 WHERE pseudo='%s' and titre='%s'", get_table_postfix(), nomJoueur, tempSqlTitre));
	

		if (joueur != NULL)
		{
			replace(tempTitre.begin(), tempTitre.end(), ' ', '-');
			joueur->m_titreSystem->titre_vector_joueur.emplace_back(tempTitre);
			joueur->ChatPacket(CHAT_TYPE_COMMAND, "TitreMessageRecompense %s", tempTitre.c_str());
		}
	}
}

// Met à jour l'interface graphique des titres.
void TitreSysteme::UpdateInterface(LPCHARACTER ch) // Met à jour l'interface des titres python.
{
	// Envoi des titres au joueur.
	const char* titrePvP;
	const char* descriptionTitre;

	if (OuvertureInterface+TempsOuvertureInterface >= get_global_time())
	{
		//ch->ChatPacket(CHAT_TYPE_INFO, "Veuillez patienter %d seconde(s) avant d'ouvrir l'interface.", ((OuvertureInterface+TempsOuvertureInterface)-get_global_time()));
		std::ostringstream oss;

		oss << "Veuillez patienter ";
		oss << ((OuvertureInterface+TempsOuvertureInterface)-get_global_time());
		oss << " seconde(s) avant d'ouvrir l'interface";
		std::string Message = oss.str();
		oss.clear();
		replace(Message.begin(), Message.end(), ' ', '-');
		ch->ChatPacket(CHAT_TYPE_COMMAND, "TitreFermer 1");
		ch->ChatPacket(CHAT_TYPE_COMMAND, "TitreMessage %s", Message.c_str());
		return;
	}
	else
	{
		OuvertureInterface = get_global_time();
	}

	if(ch->GetRaceNum() == 1 || ch->GetRaceNum() == 3 || ch->GetRaceNum() == 4 || ch->GetRaceNum() == 6) // Femme
	{
		//ch->ChatPacket(CHAT_TYPE_COMMAND, "TitreNombreTotal %d", titre_femme_vector.size());
		//ch->ChatPacket(CHAT_TYPE_COMMAND, "TitreNombreJoueur %d", ch->m_titreSystem->titre_vector_joueur.size());
	for (int i = 0; i < titre_femme_vector.size(); i++)
	{
		std::string titreString = "TitrePVP ";
		bool existant = false;
		int idExistant = 0;
		for (int k = 0; k < ch->m_titreSystem->titre_vector_joueur.size(); k++)
		{
			if (titre_femme_vector[i] == ch->m_titreSystem->titre_vector_joueur[k])
			{
				existant = true;
				idExistant = k;
			}
		}
		if (existant)
		{
			titrePvP = ch->m_titreSystem->titre_vector_joueur[idExistant].c_str();
			std::string tempTitre = (std::string) titrePvP;
			replace(tempTitre.begin(), tempTitre.end(), ' ', '-');
			titrePvP = tempTitre.c_str();
			if (titrePvP != "")
			{
				titreString += (std::string)titrePvP;
				if (test_server > 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "DEBUG Titre: Vous possédez le titre: %s", titrePvP);
				}
				ch->ChatPacket(CHAT_TYPE_COMMAND, "%s", titreString.c_str());
				bool titreExistantSex = false;
				if (titrePvP == titre_femme_vector[i])
				{
					titreExistantSex = true;
				}
				if (titreExistantSex)
				{
					std::string couleurTitreString = "TitreCouleur ";
					std::string descString = "TitreDescPVP ";
					std::string tempDesc = description_vector[i];
					replace(tempDesc.begin(), tempDesc.end(), ' ', '-');
					descString += tempDesc;
					std::string couleurTemp = titre_couleur_vector[i];
					replace(couleurTemp.begin(), couleurTemp.end(), ' ', '-');
					couleurTitreString += couleurTemp;
					couleurTitreString += titrePvP;
					
					ch->ChatPacket(CHAT_TYPE_COMMAND, "%s", descString.c_str());
					ch->ChatPacket(CHAT_TYPE_COMMAND, "TitreBonus %d-%d[SAUT]%d-%d[SAUT]%d-%d", typebonus1_vector[i], valeurbonus1_vector[i], typebonus2_vector[i], valeurbonus2_vector[i], typebonus3_vector[i], valeurbonus3_vector[i]);
					ch->ChatPacket(CHAT_TYPE_COMMAND, "%s", couleurTitreString.c_str());
					ch->ChatPacket(CHAT_TYPE_COMMAND, "TitreObjectif %d", objectif_titre_vector[i]);
					for (int b = 0; b < id_titre_objectif_vector_joueur.size(); b++)
					{
						if (id_titre_objectif_vector_joueur[b] == cache_titre_vector[i])
						{
							ch->ChatPacket(CHAT_TYPE_COMMAND, "TitreProgression %f", pourcentage_titre_objectif_vector_joueur[b]);
						}
					}
				}
			}
		}
		else
		{
			titrePvP = "????";

			std::string tempTitre = (std::string) titrePvP;
			replace(tempTitre.begin(), tempTitre.end(), ' ', '-');
			titrePvP = tempTitre.c_str();
			if (titrePvP != "")
			{
				titreString += (std::string)titrePvP;
				if (test_server > 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "DEBUG Titre: Vous ne possédez pas le titre: %s", titrePvP);
				}
				ch->ChatPacket(CHAT_TYPE_COMMAND, "%s", titreString.c_str());
				
				std::string couleurTitreString = "TitreCouleur ";
				std::string descString = "TitreDescPVP ";
				std::string tempDesc = description_lock_vector[i];
				replace(tempDesc.begin(), tempDesc.end(), ' ', '-');
				descString += tempDesc;
				std::string couleurTemp = titre_couleur_vector[i];
				replace(couleurTemp.begin(), couleurTemp.end(), ' ', '-');
				//couleurTitreString += couleurTemp;
				couleurTitreString += titrePvP;
					
				ch->ChatPacket(CHAT_TYPE_COMMAND, "%s", descString.c_str());
				ch->ChatPacket(CHAT_TYPE_COMMAND, "TitreBonus %d-%d[SAUT]%d-%d[SAUT]%d-%d", typebonus1_vector[i], valeurbonus1_vector[i], typebonus2_vector[i], valeurbonus2_vector[i], typebonus3_vector[i], valeurbonus3_vector[i]);
				ch->ChatPacket(CHAT_TYPE_COMMAND, "%s", couleurTitreString.c_str());
				ch->ChatPacket(CHAT_TYPE_COMMAND, "TitreObjectif %d", objectif_titre_vector[i]);
				for (int b = 0; b < id_titre_objectif_vector_joueur.size(); b++)
				{
					if (id_titre_objectif_vector_joueur[b] == cache_titre_vector[i])
					{
						ch->ChatPacket(CHAT_TYPE_COMMAND, "TitreProgression %d", pourcentage_titre_objectif_vector_joueur[b]);
					}
				}
			}			
		}
	}
	}

	if (ch->GetRaceNum() == 0 || ch->GetRaceNum() == 2 || ch->GetRaceNum() == 5 || ch->GetRaceNum() == 7 || ch->GetRaceNum() == 8) // Homme
	{
		//ch->ChatPacket(CHAT_TYPE_COMMAND, "TitreNombreTotal %d", titre_vector.size());
		//ch->ChatPacket(CHAT_TYPE_COMMAND, "TitreNombreJoueur %d", ch->m_titreSystem->titre_vector_joueur.size());
	for (int i = 0; i < titre_vector.size(); i++)
	{
		std::string titreString = "TitrePVP ";
		bool existant = false;
		int idExistant = 0;
		for (int k = 0; k < ch->m_titreSystem->titre_vector_joueur.size(); k++)
		{
			if (titre_vector[i] == ch->m_titreSystem->titre_vector_joueur[k])
			{
				existant = true;
				idExistant = k;
			}
		}
		if (existant)
		{
			titrePvP = ch->m_titreSystem->titre_vector_joueur[idExistant].c_str();
			std::string tempTitre = (std::string) titrePvP;
			replace(tempTitre.begin(), tempTitre.end(), ' ', '-');
			titrePvP = tempTitre.c_str();
			if (titrePvP != "")
			{
				titreString += (std::string)titrePvP;
				if (test_server > 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "DEBUG Titre: Vous possédez le titre: %s", titrePvP);
				}
				ch->ChatPacket(CHAT_TYPE_COMMAND, "%s", titreString.c_str());
				bool titreExistantSex = false;
				if (titrePvP == titre_vector[i])
				{
					titreExistantSex = true;
				}
				if (titreExistantSex)
				{
					std::string couleurTitreString = "TitreCouleur ";
					std::string descString = "TitreDescPVP ";
					std::string tempDesc = description_vector[i];
					replace(tempDesc.begin(), tempDesc.end(), ' ', '-');
					descString += tempDesc;
					std::string couleurTemp = titre_couleur_vector[i];
					replace(couleurTemp.begin(), couleurTemp.end(), ' ', '-');
					couleurTitreString += couleurTemp;
					couleurTitreString += titrePvP;
					
					ch->ChatPacket(CHAT_TYPE_COMMAND, "%s", descString.c_str());
					ch->ChatPacket(CHAT_TYPE_COMMAND, "TitreBonus %d-%d[SAUT]%d-%d[SAUT]%d-%d", typebonus1_vector[i], valeurbonus1_vector[i], typebonus2_vector[i], valeurbonus2_vector[i], typebonus3_vector[i], valeurbonus3_vector[i]);
					ch->ChatPacket(CHAT_TYPE_COMMAND, "%s", couleurTitreString.c_str());
					ch->ChatPacket(CHAT_TYPE_COMMAND, "TitreObjectif %d", objectif_titre_vector[i]);
					for (int b = 0; b < id_titre_objectif_vector_joueur.size(); b++)
					{
						if (id_titre_objectif_vector_joueur[b] == cache_titre_vector[i])
						{
							ch->ChatPacket(CHAT_TYPE_COMMAND, "TitreProgression %d", pourcentage_titre_objectif_vector_joueur[b]);
						}
					}
				}
			}
		}
		else
		{
			titrePvP = "????";

			std::string tempTitre = (std::string) titrePvP;
			replace(tempTitre.begin(), tempTitre.end(), ' ', '-');
			titrePvP = tempTitre.c_str();
			if (titrePvP != "")
			{
				titreString += (std::string)titrePvP;
				if (test_server > 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "DEBUG Titre: Vous ne possédez pas le titre: %s", titrePvP);
				}
				ch->ChatPacket(CHAT_TYPE_COMMAND, "%s", titreString.c_str());
				
				std::string couleurTitreString = "TitreCouleur ";
				std::string descString = "TitreDescPVP ";
				std::string tempDesc = description_lock_vector[i];
				replace(tempDesc.begin(), tempDesc.end(), ' ', '-');
				descString += tempDesc;
				std::string couleurTemp = titre_couleur_vector[i];
				replace(couleurTemp.begin(), couleurTemp.end(), ' ', '-');
				//couleurTitreString += couleurTemp;
				couleurTitreString += titrePvP;
					
				ch->ChatPacket(CHAT_TYPE_COMMAND, "%s", descString.c_str());
				ch->ChatPacket(CHAT_TYPE_COMMAND, "TitreBonus %d-%d[SAUT]%d-%d[SAUT]%d-%d", typebonus1_vector[i], valeurbonus1_vector[i], typebonus2_vector[i], valeurbonus2_vector[i], typebonus3_vector[i], valeurbonus3_vector[i]);
				ch->ChatPacket(CHAT_TYPE_COMMAND, "%s", couleurTitreString.c_str());
				ch->ChatPacket(CHAT_TYPE_COMMAND, "TitreObjectif %d", objectif_titre_vector[i]);
				for (int b = 0; b < id_titre_objectif_vector_joueur.size(); b++)
				{
					if (id_titre_objectif_vector_joueur[b] == cache_titre_vector[i])
					{
						ch->ChatPacket(CHAT_TYPE_COMMAND, "TitreProgression %d", pourcentage_titre_objectif_vector_joueur[b]);
					}
				}
			}			
		}
	}
	}
}


// Effectue la demande de changement de titre envoyé par le joueur.
void TitreSysteme::SelectTitrePVP(LPCHARACTER ch, std::string strNomTitre) // Permet de choisir le titre que possède le joueur.
{
	std::string tempTitre = strNomTitre;
	AFK = false;
	compteurAfk = 0;
	bool existant = false;
	for (int i = 0; i < titre_vector_joueur.size(); i++)
	{
		if (tempTitre == titre_vector_joueur[i])
		{
			existant = true;
		}
	}

	if (existant)
	{
		int idTitre = 0;
		if (GET_SEX(ch) == 0) // homme
		{
			for (int i = 0; i < titre_vector.size(); i++)
			{
				if (GET_SEX(ch) == 0) // homme
				{
					if (tempTitre == titre_vector[i])
					{
						idTitre = i;
					}
				}
				else // femme
				{
					if (tempTitre == titre_femme_vector[i])
					{
						idTitre = i;
					}
				}
			}
		}
		if (GET_SEX(ch) == 1) // femme
		{
			for (int i = 0; i < titre_femme_vector.size(); i++)
			{
				if (GET_SEX(ch) == 0) // homme
				{
					if (tempTitre == titre_vector[i])
					{
						idTitre = i;
					}
				}
				else // femme
				{
					if (tempTitre == titre_femme_vector[i])
					{
						idTitre = i;
					}
				}
			}
		}

		RetraitBonusTitre(ch);
		ch->SetTitle(idTitre); 
		ch->PointChange(POINT_TITLE, idTitre);
		std::string tempTitre = "";
		if (GET_SEX(ch) == 0) // homme
		{
			tempTitre = titre_vector[idTitre];
		}
		else // femme
		{
			tempTitre = titre_femme_vector[idTitre];
		}
		char tempSqlTitre[64+1];
		DBManager::instance().EscapeString(tempSqlTitre, sizeof(tempSqlTitre), tempTitre.c_str(), tempTitre.length());
		std::unique_ptr<SQLMsg> pmsgUpdateTitrePlayer(DBManager::instance().DirectQuery("UPDATE player%s SET titre='%d' WHERE name='%s';", get_table_postfix(), idTitre, ch->GetName()));
		std::unique_ptr<SQLMsg> pmsgUpdateTitrePlayer2(DBManager::instance().DirectQuery("UPDATE player_titre%s SET active='%d' WHERE pseudo='%s';", get_table_postfix(), 0, ch->GetName()));
		std::unique_ptr<SQLMsg> pmsgUpdateTitrePlayer3(DBManager::instance().DirectQuery("UPDATE player_titre%s SET active='%d' WHERE pseudo='%s' AND titre='%s';", get_table_postfix(), 1, ch->GetName(), tempSqlTitre));

		niveauTitreTueur = idTitre;
		ch->UpdatePacket();
		if (titre_effet_vector[idTitre] != "" && titre_effet_vector[idTitre] != "NONE")
		{
			ch->SpecificEffectPacket(titre_effet_vector[idTitre].c_str());
		}
		
		niveauTitreTueur = idTitre;
		UpdateBonusTitre(ch, idTitre);

		if (test_server > 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "DEBUG Titre: Niveau Titre choisi: %d .", idTitre);
			ch->ChatPacket(CHAT_TYPE_INFO, "DEBUG Titre: Votre titre: %s a bien été activé.", strNomTitre.c_str());
		}
	}
	else
	{
		std::string Message = "Vous ne possédez pas ce titre.";
		replace(Message.begin(), Message.end(), ' ', '-');
		ch->ChatPacket(CHAT_TYPE_COMMAND, "TitreMessage %s", Message.c_str());
	}
}

bool TitreSysteme::VerifTitre(const char* titre, const char* pseudo)
{
	for (int i = 0; i < titre_vector_joueur.size(); i++)
	{
		if (titre_vector_joueur[i] == titre)
		{
			return true;
		}
	}
	return false;
}


bool TitreSysteme::CheckTitreExiste(std::string titre)
{
	for (int i = 0; i < titre_vector.size(); i++)
	{
		if (titre_vector[i] == titre.c_str())
		{
			return true;
		}
		else if (titre_femme_vector[i] == titre.c_str())
		{
			return true;
		}
	}
	return false;
}

int TitreSysteme::GetIdTitre(std::string titre)
{
	for (int i = 0; i < titre_vector.size(); i++)
	{
		if (titre_vector[i] == titre.c_str())
		{
			return i;
		}
		if (titre_femme_vector[i] == titre.c_str())
		{
			return i;
		}
	}
	return 0;
}

int TitreSysteme::GetIdObjectifTitre(std::string titre)
{
	for (int i = 0; i < titre_vector.size(); i++)
	{
		if (titre_vector[i] == titre.c_str())
		{
			return i;
		}
		if (titre_femme_vector[i] == titre.c_str())
		{
			return i;
		}
	}
	return 0;
}

bool TitreSysteme::CheckTitreJoueurExiste(std::string titre)
{
	for (int i = 0; i < id_titre_objectif_vector_joueur.size(); i++)
	{
		if (id_titre_objectif_vector_joueur[i] == GetIdTitre(titre))
		{
			return true;
		}
	}
	return false;
}

int TitreSysteme::GetPourcentTitre(std::string titre)
{
	if (CheckTitreExiste(titre))
	{
		for (int i = 0; i < id_titre_objectif_vector_joueur.size(); i++)
		{
			if (id_titre_objectif_vector_joueur[i] == GetIdTitre(titre))
			{
				return pourcentage_titre_objectif_vector_joueur[i];
			}
		}
	}
	else
	{
		sys_err("[Système de Titre]::GetPourcentTitre le titre %s n'existe pas", titre.c_str());
	}
	return 0;
}

int TitreSysteme::GetMaxPourcentTitre(std::string titre)
{
	if (CheckTitreExiste(titre))
	{
		for (int i = 0; i < titre_vector.size(); i++)
		{
			if (titre_vector[i] == titre.c_str())
			{
				return objectif_titre_vector[i];
			}
			if (titre_femme_vector[i] == titre.c_str())
			{
				return objectif_titre_vector[i];
			}
		}
	}
	else
	{
		sys_err("[Système de Titre]::GetMaxPourcentTitre le titre %s n'existe pas", titre.c_str());
	}
	return 0;
}

void TitreSysteme::AddPourcentTitre(std::string titre, int pourcentage, LPCHARACTER ch)
{
	if (CheckTitreExiste(titre))
	{
		if(CheckTitreJoueurExiste(titre))
		{
			for (int i = 0; i < id_titre_objectif_vector_joueur.size(); i++)
			{
				if (GetIdTitre(titre) == id_titre_objectif_vector_joueur[i])
				{
					if (pourcentage_titre_objectif_vector_joueur[i] < objectif_titre_vector[GetIdObjectifTitre(titre)])
					{
						pourcentage_titre_objectif_vector_joueur[i] += pourcentage;
						std::unique_ptr<SQLMsg> pmsgUpdateObjectifTitre(DBManager::instance().DirectQuery("UPDATE player_titre_objectif%s SET pourcentage='%d' WHERE idJoueur='%d' AND id_titre='%d';", get_table_postfix(), pourcentage_titre_objectif_vector_joueur[i], ch->GetPlayerID(), id_titre_objectif_vector_joueur[i]));
					}
				}
			}
		}
		else
		{
			id_titre_objectif_vector_joueur.emplace_back(GetIdTitre(titre));
			pourcentage_titre_objectif_vector_joueur.emplace_back(pourcentage);
			std::unique_ptr<SQLMsg> pmsgInsertObjectifTitre(DBManager::instance().DirectQuery("INSERT INTO player_titre_objectif%s (id_titre, idJoueur, pourcentage) VALUES('%d', '%d', '%d');", get_table_postfix(), GetIdTitre(titre), ch->GetPlayerID(),  pourcentage));

		}
	}
	else
	{
		sys_err("[Système de Titre]::AddPourcenTitre le titre %s n'existe pas", titre.c_str());
	}
}

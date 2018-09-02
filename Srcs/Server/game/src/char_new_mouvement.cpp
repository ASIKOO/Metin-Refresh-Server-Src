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
#include "char_new_mouvement.h"
#include "db.h"
#include <sstream>
#include <iostream>
#include <string>
#include <time.h>

using namespace std;


// Evènement de mise à jour automatique du système de nouveau mouvement.
EVENTINFO(nouveau_mouvement_event_info) 
{
	LPCHARACTER ch;
};

// Fonction de mise à jour automatique du système de nouveau mouvement.
EVENTFUNC(nouveau_mouvement_update_event) 
{
	nouveau_mouvement_event_info* info = dynamic_cast<nouveau_mouvement_event_info*>( event->info );

	LPCHARACTER ch = info->ch;
	if ( info == NULL )
	{
		sys_err( "check_speedhack_event> <Factor> Null pointer" );
		return 0;
	}
	if (ch == NULL || ch->m_nouveauMouvementSystem == NULL)
	{
		return 0;
	}
	ch->m_nouveauMouvementSystem->EnvoiNouveauMouvement(ch);
 	return PASSES_PER_SEC(0.05f);
}


EVENTINFO(update_mouvement_event_info) 
{
	LPCHARACTER ch;
};

// Fonction de mise à jour automatique du système de nouveau mouvement.
EVENTFUNC(update_mouvement_update_event) 
{
	update_mouvement_event_info* info = dynamic_cast<update_mouvement_event_info*>( event->info );

	LPCHARACTER ch = info->ch;
	if ( info == NULL )
	{
		sys_err( "check_speedhack_event> <Factor> Null pointer" );
		return 0;
	}
	if (ch == NULL || ch->m_nouveauMouvementSystem == NULL)
	{
		return 0;
	}
	if (ch->m_nouveauMouvementSystem->IsTombe == false)
	{
		if (ch->m_nouveauMouvementSystem->IsVol == false && ch->m_nouveauMouvementSystem->IsNage == false && ch->m_nouveauMouvementSystem->hauteurZ - (ch->m_nouveauMouvementSystem->hauteurSol+10) > 300)
		{
			if (ch->IsDead() == false)
			{
				ch->m_nouveauMouvementSystem->totalDegats = ((ch->m_nouveauMouvementSystem->hauteurZ - ch->m_nouveauMouvementSystem->hauteurSol) - ch->GetPoint(POINT_LEVEL)) - 300;
				if (ch->m_nouveauMouvementSystem->totalDegats > 0)
				{
					ch->m_nouveauMouvementSystem->IsTombe = true;
				}
			}
		}
	}
	else
	{
		if (ch->m_nouveauMouvementSystem->IsNage)
		{
			ch->m_nouveauMouvementSystem->totalDegats = 0;
			ch->m_nouveauMouvementSystem->IsTombe = false;
		}
		else if (ch->m_nouveauMouvementSystem->IsVol)
		{
			ch->m_nouveauMouvementSystem->totalDegats = 0;
			ch->m_nouveauMouvementSystem->IsTombe = false;
		}
		else
		{
			if (ch->IsDead() == false)
			{
				if (ch->m_nouveauMouvementSystem->hauteurZ <= ch->m_nouveauMouvementSystem->hauteurSol+10)
				{
					ch->SendDamagePacket(ch, ch->m_nouveauMouvementSystem->totalDegats, (BYTE)1);
					ch->PointChange(POINT_HP, -ch->m_nouveauMouvementSystem->totalDegats);
					ch->UpdatePacket();
					ch->m_nouveauMouvementSystem->totalDegats = 0;
					ch->m_nouveauMouvementSystem->IsTombe = false;
					if (ch->GetHP() <= 0)
					{
						ch->Dead();
					}
				}
			}
			else
			{
				ch->m_nouveauMouvementSystem->totalDegats = 0;
				ch->m_nouveauMouvementSystem->IsTombe = false;
			}
		}
	}
 	return PASSES_PER_SEC(0.05f);
}


// Envoi les statuts des joueurs de nage
void NouveauMouvement::EnvoiNouveauMouvement(LPCHARACTER ch)
{
	if (ch->IsDead())
	{
		ch->m_nouveauMouvementSystem->IsVol = false;
	}
	if (ch->m_nouveauMouvementSystem->IsVol == true) // Vérifie si il vol.
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "VolSysteme %s On %f", ch->GetName(), ch->m_nouveauMouvementSystem->hauteurZ);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "VolSysteme %s Off %f", ch->GetName(), ch->m_nouveauMouvementSystem->hauteurZ);
	}

	const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();

	for (itertype(c_ref_set) it = c_ref_set.begin(); it != c_ref_set.end(); ++it)
	{
		LPCHARACTER chFound = (*it)->GetCharacter();

		if (!chFound)
			continue;

		if (chFound->IsPC())
		{
			if (chFound->m_nouveauMouvementSystem != NULL)
			{
				if (chFound->m_nouveauMouvementSystem->IsVol) // Vérifie si il vol.
				{
					ch->ChatPacket(CHAT_TYPE_COMMAND, "VolSysteme %s On %f", chFound->GetName(), chFound->m_nouveauMouvementSystem->hauteurZ);
				}
				else
				{
					ch->ChatPacket(CHAT_TYPE_COMMAND, "VolSysteme %s Off %f", chFound->GetName(), chFound->m_nouveauMouvementSystem->hauteurZ);
				}
			}
		}
	}
}

void NouveauMouvement::StartUpdateNouveauMouvement(LPCHARACTER ch)
{
	if(m_pkNouveauMouvementUpdateEvent == NULL)
	{
		nouveau_mouvement_event_info* info = AllocEventInfo<nouveau_mouvement_event_info>();
		info->ch = ch;
		m_pkNouveauMouvementUpdateEvent = event_create(nouveau_mouvement_update_event, info, PASSES_PER_SEC(0.05f));
	}
	if (m_pkNouveauMouvementUpdateStatutEvent == NULL)
	{
		update_mouvement_event_info* info = AllocEventInfo<update_mouvement_event_info>();
		info->ch = ch;
		m_pkNouveauMouvementUpdateStatutEvent = event_create(update_mouvement_update_event, info, PASSES_PER_SEC(0.05f));
	}
}

void NouveauMouvement::VolStatut(bool action)
{
	if (action)
	{
		IsVol = true;
	}
	else
	{
		IsVol = false;
	}
}

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
#include "char_bot.h"
#include "db.h"
#include <sstream>
#include <iostream>
#include <string>


using namespace std;

void CreationBot(LPCHARACTER ch)
{
	for (int i = 0; i < 300; i++)
	{
		const CMob * pkMob = CMobManager::instance().Get(101);
		LPCHARACTER newBot = CHARACTER_MANAGER::instance().CreateCharacter(pkMob->m_table.szLocaleName);
		newBot->SetEmpire(2);
		newBot->Show(ch->GetMapIndex(), ch->GetX()+number(10, 50), ch->GetY()+number(10, 50), 0, false);
		newBot->m_bCharType = 0;
		newBot->SetName("Bot n°"+number(10, 1000));
		//newBot->SetPolymorph(number(1,8), true);
		/*LPCHARACTER newBot = CHARACTER_MANAGER::instance().SpawnMob(
				number(1, 8), 
				41, 
				383+number(10, 50), 647+number(10, 50), 0,
				false, (int)(90), false);*/

	}
}

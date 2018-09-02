#include "stdafx.h"
#include "constants.h"
#include "questmanager.h"
#include "questlua.h"
//#include "dungeon.h"
//#include "unique.h"
#include "char.h"
#include "party.h"
#include "buffer_manager.h"
#include "char_manager.h"
#include "packet.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "unique.h"


#undef sys_err
#ifndef __WIN32__
#define sys_err(fmt, args...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, ##args)
#else
#define sys_err(fmt, ...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#endif

//extern CUnique 

//template <class Func> Func CUnique::ForEachMember(Func f)
//{
//	itertype(m_set_pkCharacter) it;
//
//	for (it = m_set_pkCharacter.begin(); it != m_set_pkCharacter.end(); ++it)
//	{
//		sys_log(0, "Unique ForEachMember %s", (*it)->GetName());
//		f(*it);
//	}
//	return f;
//}

LPUNIQUE master = NULL;

namespace quest
{
	int unique_init(lua_State* L)
	{
		sys_log(0,"unique_init: Init");
		//LPUNIQUE pUnique = M2_NEW CUnique(0);
		//LPUNIQUE pUnique = CUniqueManager::instance().Create();
		CUniqueManager* pUniqueManager = new CUniqueManager();
		master = pUniqueManager->Create(1);

		if(!master)
			sys_err("cannot create unique");
		sys_log(0, "created a new unique instance! yay!");

		return 0;
	}
	int unique_spawn_unique(lua_State* L)
	{
		if (!lua_isstring(L,1) || !lua_isnumber(L,2) || !lua_isstring(L,3))
			return 0;
		sys_log(0,"QUEST_unique_SPAWN_UNIQUE %s %d %s", lua_tostring(L,1), (int)lua_tonumber(L,2), lua_tostring(L,3));

		const LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();
//		LPUNIQUE pUnique = CUniqueManager::Find(1);
//		CQuestManager& q = CQuestManager::instance();
//		CUnique instancethis = CDungeonManager::instance().FindByMapIndex
//		CUniqueManager* pUniqueManager = new CUniqueManager();
//		LPUNIQUE pUnique = pUniqueManager->Find(1);
		if(!master)
		{
			sys_err("UNIQUE_SPAWN_UNIQUE: Unique class did not respond!!!");
			return 0;
		}
		if(master->CheckUniqueExists(lua_tostring(L,1))== true)
		{
			sys_log(0, "Unique already exists! %s Gonna purge..", lua_tostring(L, 1));
			master->PurgeUnique(lua_tostring(L, 1));
		}
		master->SpawnUnique(lua_tostring(L,1), (DWORD)lua_tonumber(L,2), lua_tostring(L,3), pChar);

		return 0;
	}

	int unique_set_unique(lua_State* L)
	{
		if (!lua_isstring(L, 1) || !lua_isnumber(L, 2))
			return 0;
		sys_log(0, "Starting the procedure to set unique!!");
		DWORD vid = (DWORD) lua_tonumber(L, 2);
//		CUniqueManager* pUniqueManager = new CUniqueManager();
//		LPUNIQUE pUnique = pUniqueManager->Find(1);
		if(!master)
		{
			sys_err("UNIQUE_SET_UNIQUE: Unique class did not respond!!!");
			return 0;
		}
		if(master->CheckUniqueExists(lua_tostring(L,1))== true)
		{
			sys_log(0, "Unique already exists! %s Gonna purge..", lua_tostring(L, 1));
			master->PurgeUnique(lua_tostring(L, 1));
		}
		master->SetUnique(lua_tostring(L, 1), vid);
		sys_log(0, "Unique set succesful!");
		return 0;
	}

	int unique_get_vid(lua_State* L)
	{
		if (!lua_isstring(L,1))
		{
			lua_pushnumber(L,0);
			return 1;
		}

		//CQuestManager& q = CQuestManager::instance();
//		CUniqueManager* pUniqueManager = new CUniqueManager();
//		LPUNIQUE pUnique = pUniqueManager->Find(1);

		if(master)
		{
			lua_pushnumber(L, master->GetUniqueVid(lua_tostring(L,1)));
			return 1;
		}

		lua_pushnumber(L,0);
		return 1;
	}

	int unique_check_exists(lua_State* L)
	{
		if (!lua_isstring(L,1))
		{
			sys_err("unique_check_exists: String expected. Not provided.");
			lua_pushnumber(L,0);
			return 1;
		}

		//CQuestManager& q = CQuestManager::instance();
		//CUniqueManager& Uni = CUniqueManager::instance();

		//LPUNIQUE pUnique = Uni.Find(1);
//		CUnique Uniquep = CUnique(0);
//		LPUNIQUE pUnique = &Uniquep;
//		CUniqueManager* pUniqueManager = new CUniqueManager();
//		LPUNIQUE pUnique = pUniqueManager->Find(1);
		if(master)
		{
			if(master->CheckUniqueExists(lua_tostring(L,1))== true)
			{
				lua_pushboolean(L,true);
				return 1;
			}
			sys_log(0, "unique_check_exists: Unique mob not found %s!!", lua_tostring(L,1));
			lua_pushboolean(L,false);
			return 1;
		} else {
			sys_err("unique_check_exists: Unique class did not respond!");
		}

		lua_pushboolean(L,false);
		return 0;
	}

	int unique_set_maxhp(lua_State* L)
	{
		if (!lua_isstring(L,1) || !lua_isnumber(L,2))
			return 0;

//		CQuestManager& q = CQuestManager::instance();
//		CUniqueManager* pUniqueManager = new CUniqueManager();
//		LPUNIQUE pUnique = pUniqueManager->Find(1);

		if(master)
			master->UniqueSetMaxHP(lua_tostring(L,1), (int)lua_tonumber(L,2));

		return 0;
	}

	int unique_set_hp(lua_State* L)
	{
		if (!lua_isstring(L,1) || !lua_isnumber(L,2))
			return 0;

//		CQuestManager& q = CQuestManager::instance();
//		CUniqueManager* pUniqueManager = new CUniqueManager();
//		LPUNIQUE pUnique = pUniqueManager->Find(1);

		if(master)
			master->UniqueSetHP(lua_tostring(L,1), (int)lua_tonumber(L,2));

		return 0;
	}

	int unique_set_def_grade(lua_State* L)
	{
		if (!lua_isstring(L,1) || !lua_isnumber(L,2))
			return 0;

//		CQuestManager& q = CQuestManager::instance();
//		CUniqueManager* pUniqueManager = new CUniqueManager();
//		LPUNIQUE pUnique = pUniqueManager->Find(1);

		if(master)
			master->UniqueSetDefGrade(lua_tostring(L,1), (int)lua_tonumber(L,2));

		return 0;
	}

	int unique_get_hp_perc(lua_State* L)
	{
		if (!lua_isstring(L,1))
		{
			lua_pushnumber(L,0);
			return 1;
		}

//		CQuestManager& q = CQuestManager::instance();
//		CUniqueManager* pUniqueManager = new CUniqueManager();
//		LPUNIQUE pUnique = pUniqueManager->Find(1);

		if(master)
		{
			lua_pushnumber(L, master->GetUniqueHpPerc(lua_tostring(L,1)));
			return 1;
		}

		lua_pushnumber(L,0);
		return 1;
	}

	int unique_is_dead(lua_State* L)
	{
		if (!lua_isstring(L,1))
		{
			lua_pushboolean(L, 0);
			return 1;
		}

//		CQuestManager& q = CQuestManager::instance();
//		CUniqueManager* pUniqueManager = new CUniqueManager();
//		LPUNIQUE pUnique = pUniqueManager->Find(1);

		if(master)
		{
			lua_pushboolean(L, master->IsUniqueDead(lua_tostring(L,1))?1:0);
			return 1;
		}

		lua_pushboolean(L, 0);
		return 1;
	}

	int unique_kill(lua_State* L)
	{
		if (!lua_isstring(L,1))
			return 0;
		sys_log(0,"QUEST_DUNGEON_KILL_UNIQUE %s", lua_tostring(L,1));

//		CQuestManager& q = CQuestManager::instance();
//		CUniqueManager* pUniqueManager = new CUniqueManager();
//		LPUNIQUE pUnique = pUniqueManager->Find(1);

		if(master)
		{
			const LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();
			if(!pChar)
			{
				sys_err("Attempt to kill unique monster without char!? Cowardly refusing..");
				return 0;
			}
			master->KillUnique(lua_tostring(L,1), pChar);
		}

		return 0;
	}

	int unique_purge(lua_State* L)
	{
		if (!lua_isstring(L,1))
			return 0;
		sys_log(0,"QUEST_DUNGEON_PURGE_UNIQUE %s", lua_tostring(L,1));

//		CQuestManager& q = CQuestManager::instance();
//		CUniqueManager* pUniqueManager = new CUniqueManager();
//		LPUNIQUE pUnique = pUniqueManager->Find(1);

		if(master)
			master->PurgeUnique(lua_tostring(L,1));

		return 0;
	}

	void RegisterUniqueFunctionTable() 
	{
		luaL_Reg unique_functions[] = 
		{
			{ "spawn_unique",		unique_spawn_unique	},
			{ "set_unique",			unique_set_unique	},
			{ "init",				unique_init  },
			{ "purge_unique",		unique_purge	},
			{ "kill_unique",		unique_kill	},
			{ "is_unique_dead",		unique_is_dead	},
			{ "get_hp_perc",		unique_get_hp_perc},
			{ "set_def_grade",	unique_set_def_grade},
			{ "set_hp",		unique_set_hp	},
			{ "set_maxhp",		unique_set_maxhp},
			{ "get_vid",		unique_get_vid},
			{ "exists",		unique_check_exists},

			{ NULL,				NULL			}
		};

		CQuestManager::instance().AddLuaFunctionTable("unique", unique_functions);
	}
}

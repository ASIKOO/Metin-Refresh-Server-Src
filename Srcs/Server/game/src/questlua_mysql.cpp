#include "stdafx.h"

#include "questlua.h"
#include "questmanager.h"
#include "desc_client.h"
#include "char.h"
#include "item_manager.h"
#include "item.h"
#include "cmd.h"
#include "packet.h"
#include "utils.h"
#include "db.h"

#undef sys_err
#ifndef __WIN32__
#define sys_err(fmt, args...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, ##args)
#else
#define sys_err(fmt, ...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#endif

namespace quest
{

	// misc.myqsl_update_query(string) - 'returns 0 for failed and 1 for successfull'
	int mysql_update_query(lua_State* L) {
		char szQuery[1024];
		const char * query = lua_tostring(L, 1);
		snprintf(szQuery, sizeof(szQuery), "%s",query);
		SQLMsg * msg = DBManager::instance().DirectQuery(szQuery);
		if(msg) {
			lua_pushnumber(L,1);
		} else {
			sys_err("MySQL Query failed!");
			lua_pushnumber(L,0);
		}
		M2_DELETE(msg);
		return 1;
	}

	// misc.myqsl_query(string) - 'returns a lua mysql table'
	int mysql_query(lua_State* L) {
		int i=0;
		//MYSQL_FIELD *field;
		//MYSQL_ROW row;
		char szQuery[1024];
		const char * query = lua_tostring(L, 1);
		snprintf(szQuery, sizeof(szQuery), "%s",query);
			
		SQLMsg * msg = DBManager::instance().DirectQuery(szQuery);
		if(msg) {
			std::unique_ptr<SQLMsg> pmsg(msg);
			lua_newtable(L);
			while(MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult)) {
				while(MYSQL_FIELD * field = (MYSQL_FIELD*)mysql_fetch_field(pmsg->Get()->pSQLResult) ) {
					lua_pushstring(L,field->name);
					lua_pushstring(L,row[i]);
					lua_rawset(L, -3);
					i++;            
				}
			}
			return 1;
		} else {
			sys_err("MySQL Query failed!");
			lua_pushnumber(L,0);
		}
		return 0;
	}

	int mysql_do_query(lua_State* L)
	{
		//MYSQL_FIELD *field;
		SQLMsg* run = DBManager::instance().DirectQuery(lua_tostring(L,1));
		MYSQL_RES* res=run->Get()->pSQLResult;
		if (!res){
			lua_pushnumber(L, 0);
			return 0;
		}
		MYSQL_ROW row;
		lua_newtable(L);			
		int rowcount = 1;
		while((row = mysql_fetch_row(res))){
			lua_newtable(L);
			lua_pushnumber(L, rowcount);
			lua_pushvalue(L, -2);
			lua_settable(L, -4);
			unsigned int fields = mysql_num_fields(res);
			for(unsigned int i = 0; i < fields; i++){
				lua_pushnumber(L, i + 1);
				lua_pushstring(L, row[i]);
				lua_settable(L, -3);
			}
			lua_pop(L, 1);
			rowcount++;
		}
		return 1;
	}

	// misc.myqsl_real_escape_string(string) - 'returns an escaped string'
	int mysql_real_escape_string(lua_State* L) {
		char* cescapedstring = new char[strlen(lua_tostring(L,1)) * 3 + 1];
		DBManager::instance().EscapeString(cescapedstring,strlen(lua_tostring(L,1))*2+1,lua_tostring(L,1),strlen(lua_tostring(L,1)));
		lua_pushstring(L,cescapedstring);
		return 1;
	}

	void RegistermysqlFunctionTable()
	{
		luaL_Reg mysql_functions[] = 
		{
			{"real_escape_string", mysql_real_escape_string},
			{"easy_query", mysql_do_query},
			{"query", mysql_query},
			{"update_query", mysql_update_query}, 
			{ NULL,					NULL				}
		};

		CQuestManager::instance().AddLuaFunctionTable("mysql", mysql_functions);
	}
}


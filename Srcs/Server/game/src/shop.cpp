#include "stdafx.h"
#include "../../libgame/include/grid.h"
#include "constants.h"
#include "utils.h"
#include "config.h"
#include "shop.h"
#include "desc.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "log.h"
#include "db.h"
#include "questmanager.h"
#include "monarch.h"
#include "mob_manager.h"
#include "locale_service.h"
#include "char_offlineshop.h"

/* ------------------------------------------------------------------------------------ */
CShop::CShop()
	: m_dwVnum(0), m_dwNPCVnum(0), m_pkPC(NULL)
{
	m_pGrid = M2_NEW CGrid(5, 9);
}

CShop::~CShop()
{
	TPacketGCShop pack;

	pack.header		= HEADER_GC_SHOP;
	pack.subheader	= SHOP_SUBHEADER_GC_END;
	pack.size		= sizeof(TPacketGCShop);

	Broadcast(&pack, sizeof(pack));

	GuestMapType::iterator it;

	it = m_map_guest.begin();

	while (it != m_map_guest.end())
	{
		LPCHARACTER ch = it->first;
		ch->SetShop(NULL);
		++it;
	}

	M2_DELETE(m_pGrid);
}

void CShop::SetPCShop(LPCHARACTER ch)
{
	m_pkPC = ch;
}

void CShop::SetOwnerPCShop(int id)
{
	ownerPCShop = id;
}

bool CShop::Create(DWORD dwVnum, DWORD dwNPCVnum, TShopItemTable * pTable)
{
	/*
	   if (NULL == CMobManager::instance().Get(dwNPCVnum))
	   {
	   sys_err("No such a npc by vnum %d", dwNPCVnum);
	   return false;
	   }
	 */
	sys_log(0, "SHOP #%d (Shopkeeper %d)", dwVnum, dwNPCVnum);

	m_dwVnum = dwVnum;
	m_dwNPCVnum = dwNPCVnum;

	BYTE bItemCount;

	for (bItemCount = 0; bItemCount < SHOP_HOST_ITEM_MAX_NUM; ++bItemCount)
		if (0 == (pTable + bItemCount)->vnum)
			break;

	SetShopItems(pTable, bItemCount);
	return true;
}

void CShop::SetShopItems(TShopItemTable * pTable, int bItemCount)
{
	if (bItemCount > SHOP_HOST_ITEM_MAX_NUM)
		return;

	m_pGrid->Clear();

	m_itemVector.resize(SHOP_HOST_ITEM_MAX_NUM);
	memset(&m_itemVector[0], 0, sizeof(SHOP_ITEM) * m_itemVector.size());

	for (int i = 0; i < bItemCount; ++i)
	{
		LPITEM pkItem = NULL;
		const TItemTable * item_table;

		if (m_pkPC)
		{
			pkItem = m_pkPC->GetItem(pTable->pos);

			if (!pkItem)
			{
				sys_err("cannot find item on pos (%d, %d) (name: %s)", pTable->pos.window_type, pTable->pos.cell, m_pkPC->GetName());
				continue;
			}

			item_table = pkItem->GetProto();
		}
		else
		{
			if (!pTable->vnum)
				continue;

			item_table = ITEM_MANAGER::instance().GetTable(pTable->vnum);
		}

		if (!item_table)
		{
			sys_err("Shop: no item table by item vnum #%d", pTable->vnum);
			continue;
		}

		int iPos;

		if (IsPCShop())
		{
			sys_log(0, "MyShop: use position %d", pTable->display_pos);
			iPos = pTable->display_pos;
		}
		else
			iPos = m_pGrid->FindBlank(1, item_table->bSize);

		if (iPos < 0)
		{
			sys_err("not enough shop window");
			continue;
		}

		if (!m_pGrid->IsEmpty(iPos, 1, item_table->bSize))
		{
			if (IsPCShop())
			{
				sys_err("not empty position for pc shop %s[%d]", m_pkPC->GetName(), m_pkPC->GetPlayerID());
			}
			else
			{
				sys_err("not empty position for npc shop");
			}
			continue;
		}

		m_pGrid->Put(iPos, 1, item_table->bSize);

		SHOP_ITEM & item = m_itemVector[iPos];

		item.pkItem = pkItem;
		item.itemid = 0;

		if (item.pkItem)
		{
			item.vnum = pkItem->GetVnum();
			item.count = pkItem->GetCount(); // PC 샵의 경우 아이템 개수는 진짜 아이템의 개수여야 한다.
			item.price = pTable->price; // 가격도 사용자가 정한대로..
			item.itemid	= pkItem->GetID();
		}
		else
		{
			item.vnum = pTable->vnum;
			item.count = pTable->count;

			if (IS_SET(item_table->dwFlags, ITEM_FLAG_COUNT_PER_1GOLD))
			{
				if (item_table->dwGold == 0)
					item.price = item.count;
				else
					item.price = item.count / item_table->dwGold;
			}
			else
			{
				item.price = item_table->dwGold * item.count;
			}
		}

		char name[36];
		snprintf(name, sizeof(name), "%-20s(#%-5d) (x %d)", item_table->szName, (int) item.vnum, item.count);

		sys_log(0, "SHOP_ITEM: %-36s PRICE %-5d", name, item.price);
		++pTable;
	}
}

void CShop::SetPrivateShopItems(TShopItemTable2 * pTable, int bItemCount, LPCHARACTER chOwner)
{
	if (bItemCount > SHOP_HOST_ITEM_MAX_NUM)
		return;

	m_pGrid->Clear();

	m_itemVector.resize(SHOP_HOST_ITEM_MAX_NUM);
	memset(&m_itemVector[0], 0, sizeof(SHOP_ITEM) * m_itemVector.size());

	for (int i = 0; i < bItemCount; ++i)
	{

		LPITEM pkItem = NULL;
		const TItemTable * item_table;

		pkItem = chOwner->GetItem(pTable->pos);

		if (!pkItem)
		{
			continue;
		}

		item_table = pkItem->GetProto();

		if (!item_table)
		{
			continue;
		}

		int iPos;

		iPos = pTable->display_pos;

		if (iPos < 0)
		{
			continue;
		}

		if (!m_pGrid->IsEmpty(iPos, 1, item_table->bSize))
		{
			continue;
		}
		
		if (pTable->price_type <= 0 && pTable->price_type >= 4)
		{
			chOwner->ChatPacket(CHAT_TYPE_INFO, "Monnaies incorrecte.");
			continue;
		}

		m_pGrid->Put(iPos, 1, item_table->bSize);

		SHOP_ITEM & item = m_itemVector[iPos];

		item.pkItem = pkItem;
		item.itemid = 0;

		if (item.pkItem)
		{
			item.vnum = pkItem->GetVnum();
			item.count = pkItem->GetCount();
			item.price = pTable->price;	
			item.itemid	= pkItem->GetID();
			int priceType = pTable->price_type;
#ifdef ENABLE_CHEQUE_SYSTEM
			int wonPrice = pTable->won;
#ifdef __CHANGE_LOOK_SYSTEM__
			std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("INSERT INTO private_herra_items%s SET shop_id = '%d', owner_id = '%d', pos = '%d', count = '%d', vnum = '%d', socket0 = '%d', socket1 = '%d', socket2 = '%d', attrtype0 = '%d', attrvalue0 = '%d', attrtype1 = '%d', attrvalue1 = '%d', attrtype2 = '%d', attrvalue2 = '%d', attrtype3 = '%d', attrvalue3 = '%d', attrtype4 = '%d', attrvalue4 = '%d', attrtype5 = '%d', attrvalue5 = '%d', attrtype6 = '%d', attrvalue6 = '%d', price = '%lld', price_type = '%d', won = '%d', dwVnum = '%d';", get_table_postfix(), (int)m_pkPC->GetVID(), ownerPCShop, iPos, item.count, item.vnum, pkItem->GetSocket(0), pkItem->GetSocket(1), pkItem->GetSocket(2), pkItem->GetAttributeType(0), pkItem->GetAttributeValue(0), pkItem->GetAttributeType(1), pkItem->GetAttributeValue(1), pkItem->GetAttributeType(2), pkItem->GetAttributeValue(2), pkItem->GetAttributeType(3), pkItem->GetAttributeValue(3), pkItem->GetAttributeType(4), pkItem->GetAttributeValue(4), pkItem->GetAttributeType(5), pkItem->GetAttributeValue(5), pkItem->GetAttributeType(6), pkItem->GetAttributeValue(6), item.price, priceType, wonPrice, pkItem->GetItemLook()));

#else
			std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("INSERT INTO private_herra_items%s SET shop_id = '%d', owner_id = '%d', pos = '%d', count = '%d', vnum = '%d', socket0 = '%d', socket1 = '%d', socket2 = '%d', attrtype0 = '%d', attrvalue0 = '%d', attrtype1 = '%d', attrvalue1 = '%d', attrtype2 = '%d', attrvalue2 = '%d', attrtype3 = '%d', attrvalue3 = '%d', attrtype4 = '%d', attrvalue4 = '%d', attrtype5 = '%d', attrvalue5 = '%d', attrtype6 = '%d', attrvalue6 = '%d', price = '%lld', price_type = '%d', won = '%d';", get_table_postfix(), (int)m_pkPC->GetVID(), ownerPCShop, iPos, item.count, item.vnum, pkItem->GetSocket(0), pkItem->GetSocket(1), pkItem->GetSocket(2), pkItem->GetAttributeType(0), pkItem->GetAttributeValue(0), pkItem->GetAttributeType(1), pkItem->GetAttributeValue(1), pkItem->GetAttributeType(2), pkItem->GetAttributeValue(2), pkItem->GetAttributeType(3), pkItem->GetAttributeValue(3), pkItem->GetAttributeType(4), pkItem->GetAttributeValue(4), pkItem->GetAttributeType(5), pkItem->GetAttributeValue(5), pkItem->GetAttributeType(6), pkItem->GetAttributeValue(6), item.price, priceType, wonPrice));
#endif
#else
			std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("INSERT INTO private_herra_items%s SET shop_id = '%d', owner_id = '%d', pos = '%d', count = '%d', vnum = '%d', socket0 = '%d', socket1 = '%d', socket2 = '%d', attrtype0 = '%d', attrvalue0 = '%d', attrtype1 = '%d', attrvalue1 = '%d', attrtype2 = '%d', attrvalue2 = '%d', attrtype3 = '%d', attrvalue3 = '%d', attrtype4 = '%d', attrvalue4 = '%d', attrtype5 = '%d', attrvalue5 = '%d', attrtype6 = '%d', attrvalue6 = '%d', price = '%lld', price_type = '%d';", get_table_postfix(), (int)m_pkPC->GetVID(), ownerPCShop, iPos, item.count, item.vnum, pkItem->GetSocket(0), pkItem->GetSocket(1), pkItem->GetSocket(2), pkItem->GetAttributeType(0), pkItem->GetAttributeValue(0), pkItem->GetAttributeType(1), pkItem->GetAttributeValue(1), pkItem->GetAttributeType(2), pkItem->GetAttributeValue(2), pkItem->GetAttributeType(3), pkItem->GetAttributeValue(3), pkItem->GetAttributeType(4), pkItem->GetAttributeValue(4), pkItem->GetAttributeType(5), pkItem->GetAttributeValue(5), pkItem->GetAttributeType(6), pkItem->GetAttributeValue(6), item.price, priceType));
#endif
			ITEM_MANAGER::instance().RemoveItem(pkItem);
		}
		
		++pTable;
	}

	chOwner->Save();

}

long long CShop::Buy(LPCHARACTER ch, BYTE pos)
{

	GuestMapType::iterator it = m_map_guest.find(ch);

	if (it == m_map_guest.end())
		return SHOP_SUBHEADER_GC_END;
		
	int npcVID = 0;
	int ownerVID = 0;
	if (m_pkPC != NULL && ownerPCShop != NULL)
	{
		npcVID = (int)m_pkPC->GetVID();
		ownerVID = ownerPCShop;
	}
	else
	{
		if (pos >= m_itemVector.size())
		{
			return SHOP_SUBHEADER_GC_INVALID_POS;
		}

		GuestMapType::iterator it = m_map_guest.find(ch);

		if (it == m_map_guest.end())
			return SHOP_SUBHEADER_GC_END;

		SHOP_ITEM& r_item = m_itemVector[pos];

		if (r_item.price < 0)
		{
			return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
		}

		LPITEM pkSelectedItem = ITEM_MANAGER::instance().Find(r_item.itemid);

		long long dwPrice = r_item.price;

		if (ch->GetGold() < (long long) dwPrice)
		{
			return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
		}

#ifdef ENABLE_CHEQUE_SYSTEM
		int dwWon = r_item.won;
		if (ch->GetCheque() < (int) dwWon)
		{
			return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
		}
#endif
	


		LPITEM item;

		item = ITEM_MANAGER::instance().CreateItem(r_item.vnum, r_item.count);

		if (!item)
			return SHOP_SUBHEADER_GC_SOLD_OUT;

		int iEmptyPos;
		if (item->IsDragonSoul())
		{
			iEmptyPos = ch->GetEmptyDragonSoulInventory(item);
		}
		else
		{
			iEmptyPos = ch->GetEmptyInventory(item->GetSize());
		}

		if (iEmptyPos < 0)
		{
			M2_DESTROY_ITEM(item);
			return SHOP_SUBHEADER_GC_INVENTORY_FULL;
		}

		ch->PointChange(POINT_GOLD, -dwPrice, false);

#ifdef ENABLE_CHEQUE_SYSTEM
		ch->PointChange(POINT_CHEQUE, -dwWon, false);
#endif

		if (item->IsDragonSoul())
			item->AddToCharacter(ch, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyPos));
		else
			item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
			
		ITEM_MANAGER::instance().FlushDelayedSave(item);

		ch->Save();

		return (SHOP_SUBHEADER_GC_OK);
	}
	
	if (ch->GetPlayerID() == ownerVID)
	{
		return SHOP_SUBHEADER_GC_OWNER;
	}

	char szQuery[1024+1];
	snprintf(szQuery, sizeof(szQuery), "SELECT * FROM private_herra_items%s WHERE shop_id = '%d' AND owner_id = '%d' AND status = '0' AND pos = '%d';", get_table_postfix(), npcVID, ownerVID, pos);
	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery(szQuery));

	if (msg->Get()->uiNumRows == 0)
	{
		return SHOP_SUBHEADER_GC_INVALID_POS;
	}
	else
	{
			
		MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult);
			
		int vnum;
		int count;
		long long price;
		int price_type;
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
		int applytype0;
		int applyvalue0;
		int applytype1;
		int applyvalue1;
		int applytype2;
		int applyvalue2;
		int applytype3;
		int applytype4;
		int applytype5;
		int applytype6;
		int applytype7;
		int applyvalue3;
		int applyvalue4;
		int applyvalue5;
		int applyvalue6;
		int applyvalue7;
#ifdef ENABLE_CHEQUE_SYSTEM
		int priceWon;
#endif
#ifdef __CHANGE_LOOK_SYSTEM__
		int dwVnum;
#endif
		str_to_number(vnum, row[4]);
		str_to_number(count, row[3]);
		str_to_number(price, row[25]);
		str_to_number(price_type, row[26]);
		str_to_number(socket0, row[5]);
		str_to_number(socket1, row[6]);
		str_to_number(socket2, row[7]);
		str_to_number(attrtype0, row[11]);
		str_to_number(attrvalue0, row[12]);
		str_to_number(attrtype1, row[13]);
		str_to_number(attrvalue1, row[14]);
		str_to_number(attrtype2, row[15]);
		str_to_number(attrvalue2, row[16]);
		str_to_number(attrtype3, row[17]);
		str_to_number(attrvalue3, row[18]);
		str_to_number(attrtype4, row[19]);
		str_to_number(attrvalue4, row[20]);
		str_to_number(attrtype5, row[21]);
		str_to_number(attrvalue5, row[22]);
		str_to_number(attrtype6, row[23]);
		str_to_number(attrvalue6, row[24]);
		str_to_number(applytype0, row[30]);
		str_to_number(applytype1, row[32]);
		str_to_number(applytype2, row[34]);
		str_to_number(applytype3, row[36]);
		str_to_number(applytype4, row[38]);
		str_to_number(applytype5, row[40]);
		str_to_number(applytype6, row[42]);
		str_to_number(applytype7, row[44]);
		str_to_number(applyvalue0, row[31]);
		str_to_number(applyvalue1, row[33]);
		str_to_number(applyvalue2, row[35]);
		str_to_number(applyvalue3, row[37]);
		str_to_number(applyvalue4, row[39]);
		str_to_number(applyvalue5, row[41]);
		str_to_number(applyvalue6, row[43]);
		str_to_number(applyvalue7, row[45]);
#ifdef ENABLE_CHEQUE_SYSTEM
		str_to_number(priceWon, row[46]);
#endif
#ifdef __CHANGE_LOOK_SYSTEM__
		str_to_number(dwVnum, row[47]);
#endif
		
		long long valueItem = 0;
		valueItem = ch->GetGold();
			
#ifdef ENABLE_CHEQUE_SYSTEM
		int valueWonItem = 0;
		valueWonItem = ch->GetCheque();
		if (valueItem < price)
		{
			return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
		}
		if (valueWonItem < priceWon)
		{
			return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
		}		
#else
		if (valueItem < price)
		{
			return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
		}
#endif

		if (ch->IsGM())
		{
			return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
		}


		LPITEM item;
			
		item = ITEM_MANAGER::instance().CreateItem(vnum, count);
		
		if (!item)
			return false;
			
		int iEmptyPos = ch->GetEmptyInventory(item->GetSize());

		if (iEmptyPos < 0)
		{
			M2_DESTROY_ITEM(item);
			return SHOP_SUBHEADER_GC_INVENTORY_FULL;
		}
		
		
		std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("UPDATE private_herra_items%s SET status = '1', buyer = '%s', buy_data = UNIX_TIMESTAMP() WHERE shop_id = '%d' AND owner_id = '%d' AND pos = '%d';", get_table_postfix(), ch->GetName(), npcVID, ownerVID, pos));
			
		ch->PointChange(POINT_GOLD, -price, false);
#ifdef ENABLE_CHEQUE_SYSTEM
		ch->PointChange(POINT_CHEQUE, -priceWon, false);
#endif		
		long tableSockets[3] = {socket0, socket1, socket2};
		TPlayerItemAttribute tableAttrs[15] = {{attrtype0, attrvalue0}, {attrtype1, attrvalue1}, {attrtype2, attrvalue2}, {attrtype3, attrvalue3}, {attrtype4, attrvalue4}, {attrtype5, attrvalue5}, {attrtype6, attrvalue6}, {applytype0, applyvalue0}, {applytype1, applyvalue1}, {applytype2, applyvalue2}, {applytype3, applyvalue3}, {applytype4, applyvalue4}, {applytype5, applyvalue5}, {applytype6, applyvalue6}, {applytype7, applyvalue7}};
		
		item->SetSockets(tableSockets);
		item->SetAttributes(tableAttrs);
			
#ifdef __CHANGE_LOOK_SYSTEM__
		item->SetItemLook(dwVnum);
#endif

		item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
		
		ITEM_MANAGER::instance().FlushDelayedSave(item);
		//BroadcastUpdateItem(pos, npcVID, ownerVID);
		ch->Save();
		char szQuery2[1024+1];
		snprintf(szQuery2, sizeof(szQuery2), "SELECT * FROM private_herra_items%s WHERE shop_id = '%d' AND owner_id = '%d' AND status = '0';", get_table_postfix(), npcVID, ownerVID);
		std::unique_ptr<SQLMsg> msg2(DBManager::instance().DirectQuery(szQuery2));

		if (msg2->Get()->uiNumRows <= 0)
		{
			LPCHARACTER npc = CHARACTER_MANAGER::instance().Find(npcVID);
			DBManager::instance().DirectQuery("UPDATE private_herra_shops%s SET time = UNIX_TIMESTAMP(), status = '2' WHERE shop_id = '%d' AND owner_id = '%d';", get_table_postfix(), npcVID, ownerVID);
			npc->CloseHerraShop();
		}
		return (SHOP_SUBHEADER_GC_OK);
		
	}
}


bool CShop::AddGuest(LPCHARACTER ch, DWORD owner_vid, bool bOtherEmpire)
{

	int npcVID = 0;
	int ownerVID = 0;
	if (m_pkPC != NULL && ownerPCShop != NULL)
	{
		npcVID = m_pkPC->GetVID();
		ownerVID = ownerPCShop;
	}

	if (!ch)
		return false;

	if (ch->GetExchange())
		return false;

	if (ch->GetShop())
		return false;

	ch->SetShop(this);

	m_map_guest.insert(GuestMapType::value_type(ch, bOtherEmpire));

	TPacketGCShop pack;

	pack.header		= HEADER_GC_SHOP;
	pack.subheader	= SHOP_SUBHEADER_GC_START;

	TPacketGCShopStart pack2;

	memset(&pack2, 0, sizeof(pack2));
	pack2.owner_vid = owner_vid;

	if (!IsPCShop())
	{
	
		for (DWORD i = 0; i < m_itemVector.size() && i < SHOP_HOST_ITEM_MAX_NUM; ++i)
		{
			const SHOP_ITEM & item = m_itemVector[i];

			//HIVALUE_ITEM_EVENT
			if (quest::CQuestManager::instance().GetEventFlag("hivalue_item_sell") == 0)
			{
				//축복의 구슬 && 만년한철 이벤트 
				if (item.vnum == 70024 || item.vnum == 70035)
				{				
					continue;
				}
			}
			//END_HIVALUE_ITEM_EVENT
			if (m_pkPC && !item.pkItem)
				continue;

			pack2.items[i].vnum = item.vnum;

			pack2.items[i].price = item.price;

			pack2.items[i].count = item.count;

			pack2.items[i].won = item.won;
			if (item.pkItem)
			{
				thecore_memcpy(pack2.items[i].alSockets, item.pkItem->GetSockets(), sizeof(pack2.items[i].alSockets));
				thecore_memcpy(pack2.items[i].aAttr, item.pkItem->GetAttributes(), sizeof(pack2.items[i].aAttr));
#ifdef __CHANGE_LOOK_SYSTEM__
				pack2.items[i].dwLookVnum = item.pkItem->GetItemLook();
#endif

			}
		}
		
	}
	else
	{
	
		char szQuery[1024+1];
		snprintf(szQuery, sizeof(szQuery), "SELECT * FROM private_herra_items%s WHERE shop_id = '%d' AND owner_id = '%d' AND status = '0';", get_table_postfix(), npcVID, ownerVID);
		std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery(szQuery));

		if (msg->Get()->uiNumRows != 0)
		{

			MYSQL_ROW row;
			
			while ((row = mysql_fetch_row(msg->Get()->pSQLResult)))
			{
			
				int vnum;
				int count;
				int pos;
				long long price;
				int price_type;
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
				int applytype0;
				int applytype1;
				int applytype2;
				int applytype3;
				int applytype4;
				int applytype5;
				int applytype6;
				int applytype7;
				int applyvalue0;
				int applyvalue1;
				int applyvalue2;
				int applyvalue3;
				int applyvalue4;
				int applyvalue5;
				int applyvalue6;
				int applyvalue7;
				int dwVnum;

				str_to_number(pos, row[2]);
				str_to_number(vnum, row[4]);
				str_to_number(count, row[3]);
				str_to_number(price, row[25]);
				str_to_number(price_type, row[26]);
				str_to_number(socket0, row[5]);
				str_to_number(socket1, row[6]);
				str_to_number(socket2, row[7]);
				str_to_number(attrtype0, row[11]);
				str_to_number(attrvalue0, row[12]);
				str_to_number(attrtype1, row[13]);
				str_to_number(attrvalue1, row[14]);
				str_to_number(attrtype2, row[15]);
				str_to_number(attrvalue2, row[16]);
				str_to_number(attrtype3, row[17]);
				str_to_number(attrvalue3, row[18]);
				str_to_number(attrtype4, row[19]);
				str_to_number(attrvalue4, row[20]);
				str_to_number(attrtype5, row[21]);
				str_to_number(attrvalue5, row[22]);
				str_to_number(attrtype6, row[23]);
				str_to_number(attrvalue6, row[24]);
				str_to_number(applytype0, row[30]);
				str_to_number(applytype1, row[32]);
				str_to_number(applytype2, row[34]);
				str_to_number(applytype3, row[36]);
				str_to_number(applytype4, row[38]);
				str_to_number(applytype5, row[40]);
				str_to_number(applytype6, row[42]);
				str_to_number(applytype7, row[44]);
				str_to_number(applyvalue0, row[31]);
				str_to_number(applyvalue1, row[33]);
				str_to_number(applyvalue2, row[35]);
				str_to_number(applyvalue3, row[37]);
				str_to_number(applyvalue4, row[39]);
				str_to_number(applyvalue5, row[41]);
				str_to_number(applyvalue6, row[43]);
				str_to_number(applyvalue7, row[45]);
#ifdef ENABLE_CHEQUE_SYSTEM
				int won;
				str_to_number(won, row[46]);
#endif
#ifdef __CHANGE_LOOK_SYSTEM__
				str_to_number(dwVnum, row[47]);
#endif
			
				pack2.items[pos].vnum = vnum;
				pack2.items[pos].price = price;
				pack2.items[pos].count = count;
				pack2.items[pos].price_type = price_type;

#ifdef ENABLE_CHEQUE_SYSTEM
				pack2.items[pos].won = won;
#endif
#ifdef __CHANGE_LOOK_SYSTEM__
				pack2.items[pos].dwLookVnum = dwVnum;
#endif
				
				int tableSockets[3] = {socket0, socket1, socket2};
				TPlayerItemAttribute tableAttrs[15] = {{attrtype0, attrvalue0}, {attrtype1, attrvalue1}, {attrtype2, attrvalue2}, {attrtype3, attrvalue3}, {attrtype4, attrvalue4}, {attrtype5, attrvalue5}, {attrtype6, attrvalue6}, {applytype0, applyvalue0}, {applytype1, applyvalue1}, {applytype2, applyvalue2}, {applytype3, applyvalue3}, {applytype4, applyvalue4}, {applytype5, applyvalue5}, {applytype6, applyvalue6}, {applytype7, applyvalue7}};
				
				thecore_memcpy(pack2.items[pos].alSockets, tableSockets, sizeof(pack2.items[pos].alSockets));
				thecore_memcpy(pack2.items[pos].aAttr, tableAttrs, sizeof(pack2.items[pos].aAttr));

			
			}
			
		}
		
	}

	pack.size = sizeof(pack) + sizeof(pack2);

	ch->GetDesc()->BufferedPacket(&pack, sizeof(TPacketGCShop));
	ch->GetDesc()->Packet(&pack2, sizeof(TPacketGCShopStart));
	return true;
}

void CShop::RemoveGuest(LPCHARACTER ch)
{
	if (ch->GetShop() != this)
		return;

	m_map_guest.erase(ch);
	ch->SetShop(NULL);

	TPacketGCShop pack;

	pack.header		= HEADER_GC_SHOP;
	pack.subheader	= SHOP_SUBHEADER_GC_END;
	pack.size		= sizeof(TPacketGCShop);

	ch->GetDesc()->Packet(&pack, sizeof(pack));
}

void CShop::Broadcast(const void * data, int bytes)
{
	sys_log(1, "Shop::Broadcast %p %d", data, bytes);

	GuestMapType::iterator it;

	it = m_map_guest.begin();

	while (it != m_map_guest.end())
	{
		LPCHARACTER ch = it->first;

		if (ch->GetDesc())
			ch->GetDesc()->Packet(data, bytes);

		++it;
	}
} 

void CShop::BroadcastUpdateItem(BYTE pos, int npcVID, int ownerVID)
{

	TPacketGCShop pack;
	TPacketGCShopUpdateItem pack2;

	TEMP_BUFFER	buf;

	pack.header		= HEADER_GC_SHOP;
	pack.subheader	= SHOP_SUBHEADER_GC_UPDATE_ITEM;
	pack.size		= sizeof(pack) + sizeof(pack2);

	pack2.pos		= pos;

	if (m_pkPC && !m_itemVector[pos].pkItem)
		pack2.item.vnum = 0;
	else
	{
		pack2.item.vnum	= m_itemVector[pos].vnum;
		if (m_itemVector[pos].pkItem)
		{
			thecore_memcpy(pack2.item.alSockets, m_itemVector[pos].pkItem->GetSockets(), sizeof(pack2.item.alSockets));
			thecore_memcpy(pack2.item.aAttr, m_itemVector[pos].pkItem->GetAttributes(), sizeof(pack2.item.aAttr));
		}
		else
		{
			memset(pack2.item.alSockets, 0, sizeof(pack2.item.alSockets));
			memset(pack2.item.aAttr, 0, sizeof(pack2.item.aAttr));
		}
	}

	pack2.item.price	= m_itemVector[pos].price;
	pack2.item.count	= m_itemVector[pos].count;

	buf.write(&pack, sizeof(pack));
	buf.write(&pack2, sizeof(pack2));

	Broadcast(buf.read_peek(), buf.size());

	char szQuery[1024+1];
	snprintf(szQuery, sizeof(szQuery), "SELECT * FROM private_herra_items%s WHERE shop_id = '%d' AND owner_id = '%d' AND status = '0';", get_table_postfix(), npcVID, ownerVID);
	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery(szQuery));

	sys_err("BroadcastUpdateItem PHOne: nombre d'objet restant du shop n�%d : %d", npcVID, msg->Get()->uiNumRows);
	if (msg->Get()->uiNumRows <= 0)
	{
		LPCHARACTER npc = CHARACTER_MANAGER::instance().Find(npcVID);
		DBManager::instance().DirectQuery("UPDATE private_herra_shops%s SET time = UNIX_TIMESTAMP(), status = '2' WHERE shop_id = '%d' AND owner_id = '%d';", get_table_postfix(), npcVID, ownerVID);
		npc->CloseHerraShop();
		
	}
}

int CShop::GetNumberByVnum(DWORD dwVnum)
{
	int itemNumber = 0;

	for (DWORD i = 0; i < m_itemVector.size() && i < SHOP_HOST_ITEM_MAX_NUM; ++i)
	{
		const SHOP_ITEM & item = m_itemVector[i];

		if (item.vnum == dwVnum)
		{
			itemNumber += item.count;
		}
	}

	return itemNumber;
}

bool CShop::IsSellingItem(DWORD itemID)
{
	bool isSelling = false;

	for (DWORD i = 0; i < m_itemVector.size() && i < SHOP_HOST_ITEM_MAX_NUM; ++i)
	{
		if (m_itemVector[i].itemid == itemID)
		{
			isSelling = true;
			break;
		}
	}

	return isSelling;

}

void CShop::RemoveAllGuests()
{
	if (m_map_guest.empty())
		return;
	for (GuestMapType::iterator it = m_map_guest.begin(); it != m_map_guest.end(); it++)
	{
		LPCHARACTER ch = it->first;
		if (ch)
		{
			if (ch->GetDesc() && ch->GetShop() == this)
			{
				ch->SetShop(NULL);

				TPacketGCShop pack;

				pack.header = HEADER_GC_SHOP;
				pack.subheader = SHOP_SUBHEADER_GC_END;
				pack.size = sizeof(TPacketGCShop);

				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
		}
	}
	m_map_guest.clear();
}


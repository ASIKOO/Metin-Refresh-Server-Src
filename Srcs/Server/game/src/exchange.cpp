#include "stdafx.h"
#include "../../libgame/include/grid.h"
#include "utils.h"
#include "desc.h"
#include "desc_client.h"
#include "char.h"
#include "item.h"
#include "item_manager.h"
#include "packet.h"
#include "log.h"
#include "db.h"
#include "locale_service.h"
#include "../../common/length.h"
#include "exchange.h"
#include "DragonSoul.h"
#include "config.h"

#ifdef __CHANGE_LOOK_SYSTEM__
void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, DWORD arg1, TItemPos arg2, DWORD arg3, void * pvData = NULL, DWORD dwLookVnum = 0);
#else
void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, DWORD arg1, TItemPos arg2, DWORD arg3, void * pvData = NULL);
#endif
extern bool bigger_inventory;
extern bool trade_effect;
extern int trade_effect_exchange_threshold;

// ±³È¯ ÆÐÅ¶
#ifdef __CHANGE_LOOK_SYSTEM__
void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, DWORD arg1, TItemPos arg2, DWORD arg3, void * pvData, DWORD dwLookVnum)
#else
void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, DWORD arg1, TItemPos arg2, DWORD arg3, void * pvData)
#endif
{
	if (!ch->GetDesc())
		return;

	struct packet_exchange pack_exchg;

	pack_exchg.header 		= HEADER_GC_EXCHANGE;
	pack_exchg.sub_header 	= sub_header;
	pack_exchg.is_me		= is_me;
	pack_exchg.arg1		= arg1;
	pack_exchg.arg2		= arg2;
	pack_exchg.arg3		= arg3;
#ifdef __CHANGE_LOOK_SYSTEM__
	pack_exchg.dwLookVnum	= dwLookVnum;
#endif

	if (sub_header == EXCHANGE_SUBHEADER_GC_ITEM_ADD && pvData)
	{
		thecore_memcpy(&pack_exchg.alSockets, ((LPITEM) pvData)->GetSockets(), sizeof(pack_exchg.alSockets));
		thecore_memcpy(&pack_exchg.aAttr, ((LPITEM) pvData)->GetAttributes(), sizeof(pack_exchg.aAttr));
	}
	else
	{
		memset(&pack_exchg.alSockets, 0, sizeof(pack_exchg.alSockets));
		memset(&pack_exchg.aAttr, 0, sizeof(pack_exchg.aAttr));
	}

	ch->GetDesc()->Packet(&pack_exchg, sizeof(pack_exchg));
}

// ±³È¯À» ½ÃÀÛ
bool CHARACTER::ExchangeStart(LPCHARACTER victim)
{
	if (this == victim)	// ÀÚ±â ÀÚ½Å°ú´Â ±³È¯À» ¸øÇÑ´Ù.
		return false;

	if (IsObserverMode())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°üÀü »óÅÂ¿¡¼­´Â ±³È¯À» ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return false;
	}

	if (victim->IsNPC())
		return false;

	//PREVENT_TRADE_WINDOW
	if (IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || IsAcceOpen())
	{
		ChatPacket( CHAT_TYPE_INFO, LC_TEXT("´Ù¸¥ °Å·¡Ã¢ÀÌ ¿­·ÁÀÖÀ»°æ¿ì °Å·¡¸¦ ÇÒ¼ö ¾ø½À´Ï´Ù." ) );
		return false;
	}

	if (victim->IsOpenSafebox() || victim->GetShopOwner() || victim->GetMyShop() || victim->IsCubeOpen() || victim->IsAcceOpen())
	{
		ChatPacket( CHAT_TYPE_INFO, LC_TEXT("»ó´ë¹æÀÌ ´Ù¸¥ °Å·¡ÁßÀÌ¶ó °Å·¡¸¦ ÇÒ¼ö ¾ø½À´Ï´Ù." ) );
		return false;
	}
	//END_PREVENT_TRADE_WINDOW
	int iDist = DISTANCE_APPROX(GetX() - victim->GetX(), GetY() - victim->GetY());

	// °Å¸® Ã¼Å©
	if (iDist >= EXCHANGE_MAX_DISTANCE)
		return false;

	if (GetExchange())
		return false;

	if (victim->GetExchange())
	{
		exchange_packet(this, EXCHANGE_SUBHEADER_GC_ALREADY, 0, 0, NPOS, 0);
		return false;
	}

	if (victim->IsBlockMode(BLOCK_EXCHANGE))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»ó´ë¹æÀÌ ±³È¯ °ÅºÎ »óÅÂÀÔ´Ï´Ù."));
		return false;
	}

	SetExchange(M2_NEW CExchange(this));
	victim->SetExchange(M2_NEW CExchange(victim));

	victim->GetExchange()->SetCompany(GetExchange());
	GetExchange()->SetCompany(victim->GetExchange());

	//
	SetExchangeTime();
	victim->SetExchangeTime();

	exchange_packet(victim, EXCHANGE_SUBHEADER_GC_START, 0, GetVID(), NPOS, 0);
	exchange_packet(this, EXCHANGE_SUBHEADER_GC_START, 0, victim->GetVID(), NPOS, 0);

	return true;
}

CExchange::CExchange(LPCHARACTER pOwner)
{
	m_pCompany = NULL;

	m_bAccept = false;

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		m_apItems[i] = NULL;
		m_aItemPos[i] = NPOS;
		m_abItemDisplayPos[i] = 0;
	}

	m_lGold = 0;
	m_lWon = 0;
	m_pOwner = pOwner;
	pOwner->SetExchange(this);

	m_pGrid = M2_NEW CGrid(4,3);
}

CExchange::~CExchange()
{
	M2_DELETE(m_pGrid);
}

bool CExchange::AddItem(TItemPos item_pos, BYTE display_pos)
{
	assert(m_pOwner != NULL && GetCompany());

	if (!item_pos.IsValidItemPosition())
		return false;

	// Àåºñ´Â ±³È¯ÇÒ ¼ö ¾øÀ½
	if (item_pos.IsEquipPosition())
		return false;

	LPITEM item;

	if (!(item = m_pOwner->GetItem(item_pos)))
		return false;

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_GIVE))
	{
		m_pOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÀÌÅÛÀ» °Ç³×ÁÙ ¼ö ¾ø½À´Ï´Ù."));
		return false;
	}

	if (true == item->isLocked())
	{
		return false;
	}

	// ÀÌ¹Ì ±³È¯Ã¢¿¡ Ãß°¡µÈ ¾ÆÀÌÅÛÀÎ°¡?
	if (item->IsExchanging())
	{
		sys_log(0, "EXCHANGE under exchanging");
		return false;
	}

	if (!m_pGrid->IsEmpty(display_pos, 1, item->GetSize()))
	{
		sys_log(0, "EXCHANGE not empty item_pos %d %d %d", display_pos, 1, item->GetSize());
		return false;
	}

	Accept(false);
	GetCompany()->Accept(false);

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (m_apItems[i])
			continue;

		m_apItems[i]		= item;
		m_aItemPos[i]		= item_pos;
		m_abItemDisplayPos[i]	= display_pos;
		m_pGrid->Put(display_pos, 1, item->GetSize());

		item->SetExchanging(true);

		exchange_packet(m_pOwner, 
				EXCHANGE_SUBHEADER_GC_ITEM_ADD,
				true,
				item->GetVnum(),
				TItemPos(RESERVED_WINDOW, display_pos),
				item->GetCount(),
#ifdef __CHANGE_LOOK_SYSTEM__
				item, item->GetItemLook());
#else
				item);
#endif

		exchange_packet(GetCompany()->GetOwner(),
				EXCHANGE_SUBHEADER_GC_ITEM_ADD, 
				false, 
				item->GetVnum(),
				TItemPos(RESERVED_WINDOW, display_pos),
				item->GetCount(),
#ifdef __CHANGE_LOOK_SYSTEM__
				item, item->GetItemLook());
#else
				item);
#endif

		sys_log(0, "EXCHANGE AddItem success %s pos(%d, %d) %d", item->GetName(), item_pos.window_type, item_pos.cell, display_pos);

		return true;
	}

	// Ãß°¡ÇÒ °ø°£ÀÌ ¾øÀ½
	return false;
}

bool CExchange::RemoveItem(BYTE pos)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return false;

	if (!m_apItems[pos])
		return false;

	TItemPos PosOfInventory = m_aItemPos[pos];
	m_apItems[pos]->SetExchanging(false);

	m_pGrid->Get(m_abItemDisplayPos[pos], 1, m_apItems[pos]->GetSize());

	exchange_packet(GetOwner(),	EXCHANGE_SUBHEADER_GC_ITEM_DEL, true, pos, NPOS, 0);
	exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_ITEM_DEL, false, pos, PosOfInventory, 0);

	Accept(false);
	GetCompany()->Accept(false);

	m_apItems[pos]	    = NULL;
	m_aItemPos[pos]	    = NPOS;
	m_abItemDisplayPos[pos] = 0;
	return true;
}

bool CExchange::AddGold(long gold)
{
	if (gold <= 0)
		return false;

	if (GetOwner()->GetGold() < (long long)gold)
	{
		// °¡Áö°í ÀÖ´Â µ·ÀÌ ºÎÁ·.
		exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_LESS_GOLD, 0, 0, NPOS, 0);
		return false;
	}

	LPCHARACTER	victim = GetCompany()->GetOwner();

	if(m_lGold)
	{
		long long vic_gold = victim->GetGold() + m_lGold;
		if(vic_gold > yang_max)
		{
			exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_LESS_GOLD, 0, 0, NPOS, 0);
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This user can not accept that much gold."));
			return false;
		}
	}

	if ( LC_IsCanada() == true || LC_IsEurope() == true )
	{
		if ( m_lGold > 0 )
		{
			return false;
		}
	}

	Accept(false);
	GetCompany()->Accept(false);

	m_lGold = gold;

	exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_GOLD_ADD, true, m_lGold, NPOS, 0);
	exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_GOLD_ADD, false, m_lGold, NPOS, 0);
	return true;
}

#ifdef ENABLE_CHEQUE_SYSTEM
bool CExchange::AddWon(int won)
{
	if (won <= 0)
		return false;

	if (GetOwner()->GetCheque() < (int)won)
	{
		// °¡Áö°í ÀÖ´Â µ·ÀÌ ºÎÁ·.
		exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_LESS_GOLD, 0, 0, NPOS, 0);
		return false;
	}

	LPCHARACTER	victim = GetCompany()->GetOwner();

	if(m_lWon)
	{
		int vic_won = victim->GetCheque() + m_lWon;
		if(vic_won > yang_max)
		{
			exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_LESS_GOLD, 0, 0, NPOS, 0);
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This user can not accept that much gold."));
			return false;
		}
	}

	if ( LC_IsCanada() == true || LC_IsEurope() == true )
	{
		if ( m_lWon > 0 )
		{
			return false;
		}
	}

	Accept(false);
	GetCompany()->Accept(false);

	m_lWon = won;

	exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_WON_ADD, true, m_lWon, NPOS, 0);
	exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_WON_ADD, false, m_lWon, NPOS, 0);
	return true;
}
#endif

// µ·ÀÌ ÃæºÐÈ÷ ÀÖ´ÂÁö, ±³È¯ÇÏ·Á´Â ¾ÆÀÌÅÛÀÌ ½ÇÁ¦·Î ÀÖ´ÂÁö È®ÀÎ ÇÑ´Ù.
bool CExchange::Check(int * piItemCount)
{
	if (GetOwner()->GetGold() < m_lGold)
		return false;

	int item_count = 0;

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (!m_apItems[i])
			continue;

		if (!m_aItemPos[i].IsValidItemPosition())
			return false;

		if (m_apItems[i] != GetOwner()->GetItem(m_aItemPos[i]))
			return false;

		++item_count;
	}

	*piItemCount = item_count;
	return true;
}

bool CExchange::CheckSpace()
{
	static CGrid s_grid1(5, INVENTORY_MAX_NUM / 5 / 4); // inven page 1   9 Rows a 5 Columns
	static CGrid s_grid2(5, INVENTORY_MAX_NUM / 5 / 4); // inven page 2   9 Rows a 5 Columns
	static CGrid s_grid3(5, INVENTORY_MAX_NUM / 5 / 4); // inven page 3   9 Rows a 5 Columns
	static CGrid s_grid4(5, INVENTORY_MAX_NUM / 5 / 4); // inven page 4   9 Rows a 5 Columns

	s_grid1.Clear();
	s_grid2.Clear();
	s_grid3.Clear();
	s_grid4.Clear();

	LPCHARACTER	victim = GetCompany()->GetOwner();
	LPITEM item;

	int i;

	const int perPageSlotCount = INVENTORY_MAX_NUM / 4;

	for (i = 0; i < INVENTORY_MAX_NUM; ++i) {
		if (!(item = victim->GetInventoryItem(i)))
			continue;

		BYTE itemSize = item->GetSize();

		if (i < perPageSlotCount) // Notice: This is adjusted for 4 Pages only!
			s_grid1.Put(i, 1, itemSize);
		else if (i < perPageSlotCount * 2)
			s_grid2.Put(i - perPageSlotCount, 1, itemSize);
		else if (i < perPageSlotCount * 3)
			s_grid3.Put(i - perPageSlotCount * 2, 1, itemSize);
		else
			s_grid4.Put(i - perPageSlotCount * 3, 1, itemSize);
	}

	static std::vector <WORD> s_vDSGrid(DRAGON_SOUL_INVENTORY_MAX_NUM);

	bool bDSInitialized = false;

	for (i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (!(item = m_apItems[i]))
			continue;

		BYTE itemSize = item->GetSize();

		if (item->IsDragonSoul())
		{
			if (!victim->DragonSoul_IsQualified())
				return false;

			if (!bDSInitialized) {
				bDSInitialized = true;
				victim->CopyDragonSoulItemGrid(s_vDSGrid);
			}

			bool bExistEmptySpace = false;
			WORD wBasePos = DSManager::instance().GetBasePosition(item);
			if (wBasePos >= DRAGON_SOUL_INVENTORY_MAX_NUM)
				return false;

			for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; i++)
			{
				WORD wPos = wBasePos + i;
				if (0 == s_vDSGrid[wBasePos])
				{
					bool bEmpty = true;
					for (int j = 1; j < item->GetSize(); j++)
					{
						if (s_vDSGrid[wPos + j * DRAGON_SOUL_BOX_COLUMN_NUM])
						{
							bEmpty = false;
							break;
						}
					}
					if (bEmpty)
					{
						for (int j = 0; j < item->GetSize(); j++)
						{
							s_vDSGrid[wPos + j * DRAGON_SOUL_BOX_COLUMN_NUM] = wPos + 1;
						}
						bExistEmptySpace = true;
						break;
					}
				}
				if (bExistEmptySpace)
					break;
			}
			if (!bExistEmptySpace)
				return false;
		}
		else
		{
			int iPos = s_grid1.FindBlank(1, itemSize);
			if (iPos >= 0) {
				s_grid1.Put(iPos, 1, itemSize);
				continue;
			}

			iPos = s_grid2.FindBlank(1, itemSize);
			if (iPos >= 0) {
				s_grid2.Put(iPos, 1, itemSize);
				continue;
			}

			iPos = s_grid3.FindBlank(1, itemSize);
			if (iPos >= 0) {
				s_grid3.Put(iPos, 1, itemSize);
				continue;
			}

			iPos = s_grid4.FindBlank(1, itemSize);
			if (iPos >= 0) {
				s_grid4.Put(iPos, 1, itemSize);
				continue;
			}

			return false;  // No space left in inventory
		}
	}

	return true;
}

// ±³È¯ ³¡ (¾ÆÀÌÅÛ°ú µ· µîÀ» ½ÇÁ¦·Î ¿Å±ä´Ù)
bool CExchange::Done()
{
	int		empty_pos, i;
	LPITEM	item;

	LPCHARACTER	victim = GetCompany()->GetOwner();

	time_t now = time(0);
										

	char* dt = ctime(&now);
	char szCreateDate[256];
	struct tm * tm1;
	tm1 = localtime(&now);
    	strftime(szCreateDate, 255, "%Y-%m-%d %H:%M:%S", tm1);
	const char* dateLog = std::string(szCreateDate).c_str();

	if(m_lGold)
	{
		long long vic_gold = victim->GetGold() + m_lGold;
		if(vic_gold > yang_max)
		{
//			exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_LESS_GOLD, 0, 0, NPOS, 0);
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This user can not accept that much gold."));
			return false;
		}
	}

#ifdef ENABLE_CHEQUE_SYSTEM
	if(m_lWon)
	{
		int vic_won = victim->GetCheque() + m_lWon;
		if(vic_won > yang_max)
		{
//			exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_LESS_GOLD, 0, 0, NPOS, 0);
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This user can not accept that much gold."));
			return false;
		}
	}
#endif

	if(trade_effect)
		GetOwner()->CreateFly(6,victim);

	for (i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (!(item = m_apItems[i]))
			continue;

		if (item->IsDragonSoul())
			empty_pos = victim->GetEmptyDragonSoulInventory(item);
		else
			empty_pos = victim->GetEmptyInventory(item->GetSize());

		if (empty_pos < 0)
		{
			sys_err("Exchange::Done : Cannot find blank position in inventory %s <-> %s item %s", 
					m_pOwner->GetName(), victim->GetName(), item->GetName());
			continue;
		}

		assert(empty_pos >= 0);

		if (item->GetVnum() == 90008 || item->GetVnum() == 90009) // VCARD
		{
			VCardUse(m_pOwner, victim, item);
			continue;
		}

		m_pOwner->SyncQuickslot(QUICKSLOT_TYPE_ITEM, item->GetCell(), 255);

		item->RemoveFromCharacter();
		if (item->IsDragonSoul())
			item->AddToCharacter(victim, TItemPos(DRAGON_SOUL_INVENTORY, empty_pos));
		else
			item->AddToCharacter(victim, TItemPos(INVENTORY, empty_pos));
		ITEM_MANAGER::instance().FlushDelayedSave(item);
		if(trade_effect)
			GetOwner()->CreateFly(5,victim);

		std::string NomObjet = item->GetName(); // Stock le nom de l'objet.
		char tempNomObjet[1024+1]; // Créer un tableau de char d'une taille égale au nom de l'objet stocké.
		replace(NomObjet.begin(), NomObjet.end(), '\'', ' ');
		replace(NomObjet.begin(), NomObjet.end(), '\"', ' ');
		DBManager::instance().EscapeString(tempNomObjet, sizeof(tempNomObjet), NomObjet.c_str(), NomObjet.length()); // Convertit les caractères spéciaux tels que ' " etc.. en valeur insérable à MySQL.
		std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("INSERT INTO player_echange%s (pseudo, pseudo2, item, yang, won, map_index, date) VALUES('%s', '%s', '%s', 0, 0, '%d', '%s')", get_table_postfix(), GetOwner()->GetName(), victim->GetName(), tempNomObjet, GetOwner()->GetMapIndex(), dateLog));

		item->SetExchanging(false);
		{
		}

		m_apItems[i] = NULL;
	}

	if (m_lGold)
	{
		std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("INSERT INTO player_echange%s (pseudo, pseudo2, item, yang, won, map_index, date) VALUES('%s', '%s', '%s', '%lld', 0, '%d', '%s')", get_table_postfix(), GetOwner()->GetName(), victim->GetName(), "YANG", m_lGold, GetOwner()->GetMapIndex(), dateLog)); // Récupère le log d'échange
		if(trade_effect)
			for (int i = 1; i < m_lGold+1; i=i+trade_effect_exchange_threshold) 
				GetOwner()->CreateFly(4,victim);  
		GetOwner()->PointChange(POINT_GOLD, -m_lGold, true);
		victim->PointChange(POINT_GOLD, m_lGold, true);

	}

#ifdef ENABLE_CHEQUE_SYSTEM
	if (m_lWon)
	{
		std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("INSERT INTO player_echange%s (pseudo, pseudo2, item, yang, won, map_index, date) VALUES('%s', '%s', '%s', 0, '%d', '%d', '%s')", get_table_postfix(), GetOwner()->GetName(), victim->GetName(), "WON", m_lWon, GetOwner()->GetMapIndex(), dateLog)); // Récupère le log d'échange
		if(trade_effect)
			for (int i = 1; i < m_lWon+1; i=i+trade_effect_exchange_threshold) 
				GetOwner()->CreateFly(4,victim);  
		GetOwner()->PointChange(POINT_CHEQUE, -m_lWon, true);
		victim->PointChange(POINT_CHEQUE, m_lWon, true);
	}
#endif

	m_pGrid->Clear();
	return true;
}

// ±³È¯À» µ¿ÀÇ
bool CExchange::Accept(bool bAccept)
{
	if (m_bAccept == bAccept)
		return true;

	m_bAccept = bAccept;

	// µÑ ´Ù µ¿ÀÇ ÇßÀ¸¹Ç·Î ±³È¯ ¼º¸³
	if (m_bAccept && GetCompany()->m_bAccept)
	{
		int	iItemCount;

		LPCHARACTER victim = GetCompany()->GetOwner();

		//PREVENT_PORTAL_AFTER_EXCHANGE
		GetOwner()->SetExchangeTime();
		victim->SetExchangeTime();		
		//END_PREVENT_PORTAL_AFTER_EXCHANGE

		// exchange_check ¿¡¼­´Â ±³È¯ÇÒ ¾ÆÀÌÅÛµéÀÌ Á¦ÀÚ¸®¿¡ ÀÖ³ª È®ÀÎÇÏ°í,
		// ¿¤Å©µµ ÃæºÐÈ÷ ÀÖ³ª È®ÀÎÇÑ´Ù, µÎ¹øÂ° ÀÎÀÚ·Î ±³È¯ÇÒ ¾ÆÀÌÅÛ °³¼ö
		// ¸¦ ¸®ÅÏÇÑ´Ù.
		if (!Check(&iItemCount))
		{
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("µ·ÀÌ ºÎÁ·ÇÏ°Å³ª ¾ÆÀÌÅÛÀÌ Á¦ÀÚ¸®¿¡ ¾ø½À´Ï´Ù."));
			victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»ó´ë¹æÀÇ µ·ÀÌ ºÎÁ·ÇÏ°Å³ª ¾ÆÀÌÅÛÀÌ Á¦ÀÚ¸®¿¡ ¾ø½À´Ï´Ù."));
			goto EXCHANGE_END;
		}

		// ¸®ÅÏ ¹ÞÀº ¾ÆÀÌÅÛ °³¼ö·Î »ó´ë¹æÀÇ ¼ÒÁöÇ°¿¡ ³²Àº ÀÚ¸®°¡ ÀÖ³ª È®ÀÎÇÑ´Ù.
		if (!CheckSpace())
		{
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»ó´ë¹æÀÇ ¼ÒÁöÇ°¿¡ ºó °ø°£ÀÌ ¾ø½À´Ï´Ù."));
			victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÁöÇ°¿¡ ºó °ø°£ÀÌ ¾ø½À´Ï´Ù."));
			goto EXCHANGE_END;
		}

		// »ó´ë¹æµµ ¸¶Âù°¡Áö·Î..
		if (!GetCompany()->Check(&iItemCount))
		{
			victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("µ·ÀÌ ºÎÁ·ÇÏ°Å³ª ¾ÆÀÌÅÛÀÌ Á¦ÀÚ¸®¿¡ ¾ø½À´Ï´Ù."));
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»ó´ë¹æÀÇ µ·ÀÌ ºÎÁ·ÇÏ°Å³ª ¾ÆÀÌÅÛÀÌ Á¦ÀÚ¸®¿¡ ¾ø½À´Ï´Ù."));
			goto EXCHANGE_END;
		}

		if (!GetCompany()->CheckSpace())
		{
			victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»ó´ë¹æÀÇ ¼ÒÁöÇ°¿¡ ºó °ø°£ÀÌ ¾ø½À´Ï´Ù."));
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÁöÇ°¿¡ ºó °ø°£ÀÌ ¾ø½À´Ï´Ù."));
			goto EXCHANGE_END;
		}

		if (db_clientdesc->GetSocket() == INVALID_SOCKET)
		{
			sys_err("Cannot use exchange feature while DB cache connection is dead.");
			victim->ChatPacket(CHAT_TYPE_INFO, "Unknown error");
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, "Unknown error");
			goto EXCHANGE_END;
		}

		if (Done())
		{
			if (m_lGold) // µ·ÀÌ ÀÖÀ» ‹š¸¸ ÀúÀå
				GetOwner()->Save();

			if (GetCompany()->Done())
			{
				if (GetCompany()->m_lGold) // µ·ÀÌ ÀÖÀ» ¶§¸¸ ÀúÀå
					victim->Save();

				// INTERNATIONAL_VERSION
				GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ´Ô°úÀÇ ±³È¯ÀÌ ¼º»ç µÇ¾ú½À´Ï´Ù."), victim->GetName());
				victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ´Ô°úÀÇ ±³È¯ÀÌ ¼º»ç µÇ¾ú½À´Ï´Ù."), GetOwner()->GetName());
				// END_OF_INTERNATIONAL_VERSION
			}
		}

EXCHANGE_END:
		Cancel();
		return false;
	}
	else
	{
		// ¾Æ´Ï¸é accept¿¡ ´ëÇÑ ÆÐÅ¶À» º¸³»ÀÚ.
		exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_ACCEPT, true, m_bAccept, NPOS, 0);
		exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_ACCEPT, false, m_bAccept, NPOS, 0);
		return true;
	}
}

// ±³È¯ Ãë¼Ò
void CExchange::Cancel()
{
	exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_END, 0, 0, NPOS, 0);
	GetOwner()->SetExchange(NULL);

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (m_apItems[i])
			m_apItems[i]->SetExchanging(false);
	}

	if (GetCompany())
	{
		GetCompany()->SetCompany(NULL);
		GetCompany()->Cancel();
	}

	M2_DELETE(this);
}


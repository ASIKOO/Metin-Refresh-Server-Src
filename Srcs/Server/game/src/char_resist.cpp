#include "stdafx.h"
#include "constants.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "affect.h"
#include "locale_service.h"
#include "item.h"

// ��
const int poison_damage_rate[MOB_RANK_MAX_NUM] = 
{
	80, 50, 40, 30, 25, 1
};

int GetPoisonDamageRate(LPCHARACTER ch)
{
	int iRate;

	if (ch->IsPC())
	{
		if (LC_IsYMIR())
			iRate = 40;
		else
			iRate = 50;
	}
	else
		iRate = poison_damage_rate[ch->GetMobRank()];

	iRate = MAX(0, iRate - ch->GetPoint(POINT_POISON_REDUCE));
	return iRate;
}

const int bleed_damage_rate[MOB_RANK_MAX_NUM] =
{
	80, 50, 40, 30, 25, 1
};

int GetBleedDamageRate(LPCHARACTER ch)
{
	int iRate;

	if (ch->IsPC())
	{
		if (LC_IsYMIR())
			iRate = 40;
		else
			iRate = 50;
	}
	else
		iRate = poison_damage_rate[ch->GetMobRank()];

	iRate = MAX(0, iRate - ch->GetPoint(POINT_BLEEDING_REDUCE));
	return iRate;
}

EVENTINFO(TBleedEventInfo)
{
	DynamicCharacterPtr ch;
	int		count;
	DWORD	attacker_pid;

	TBleedEventInfo()
		: ch()
		, count(0)
		, attacker_pid(0)
	{
	}
};

EVENTFUNC(bleed_event)
{
	TBleedEventInfo * info = dynamic_cast<TBleedEventInfo *>(event->info);

	if (info == NULL)
	{
		sys_err("bleed_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}
	LPCHARACTER pkAttacker = CHARACTER_MANAGER::instance().FindByPID(info->attacker_pid);

	int dam = ch->GetMaxHP() * GetBleedDamageRate(ch) / 1000;
	if (test_server) ch->ChatPacket(CHAT_TYPE_NOTICE, "Bleed Damage %d", dam);

	if (ch->Damage(pkAttacker, dam, DAMAGE_TYPE_POISON))
	{
		ch->m_pkBleedEvent = NULL;
		return 0;
	}

	--info->count;

	if (info->count)
		return PASSES_PER_SEC(3);
	else
	{
		ch->m_pkBleedEvent = NULL;
		return 0;
	}
}

EVENTINFO(TPoisonEventInfo)
{
	DynamicCharacterPtr ch;
	int		count;
	DWORD	attacker_pid;

	TPoisonEventInfo()
	: ch()
	, count(0)
	, attacker_pid(0)
	{
	}
};

EVENTFUNC(poison_event)
{
	TPoisonEventInfo * info = dynamic_cast<TPoisonEventInfo *>( event->info );
	
	if ( info == NULL )
	{
		sys_err( "poison_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}
	LPCHARACTER pkAttacker = CHARACTER_MANAGER::instance().FindByPID(info->attacker_pid);

	int dam = ch->GetMaxHP() * GetPoisonDamageRate(ch) / 1000;
	if (test_server) ch->ChatPacket(CHAT_TYPE_NOTICE, "Poison Damage %d", dam);

	if (ch->Damage(pkAttacker, dam, DAMAGE_TYPE_POISON))
	{
		ch->m_pkPoisonEvent = NULL;
		return 0;
	}

	--info->count;

	if (info->count)
		return PASSES_PER_SEC(3);
	else
	{
		ch->m_pkPoisonEvent = NULL;
		return 0;
	}
}

EVENTINFO(TFireEventInfo)
{
	DynamicCharacterPtr ch;
	int		count;
	int		amount;
	DWORD	attacker_pid;

	TFireEventInfo()
	: ch()
	, count(0)
	, amount(0)
	, attacker_pid(0)
	{
	}
};

EVENTFUNC(fire_event)
{
	TFireEventInfo * info = dynamic_cast<TFireEventInfo *>( event->info );

	if ( info == NULL )
	{
		sys_err( "fire_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = info->ch;
	if (ch == NULL) { // <Factor>
		return 0;
	}
	LPCHARACTER pkAttacker = CHARACTER_MANAGER::instance().FindByPID(info->attacker_pid);

	int dam = info->amount;
	if (test_server) ch->ChatPacket(CHAT_TYPE_NOTICE, "Fire Damage %d", dam);

	if (ch->Damage(pkAttacker, dam, DAMAGE_TYPE_FIRE))
	{
		ch->m_pkFireEvent = NULL;
		return 0;
	}

	--info->count;

	if (info->count)
		return PASSES_PER_SEC(3);
	else
	{
		ch->m_pkFireEvent = NULL;
		return 0;
	}
}

/*

   LEVEL�� ����..

   +8   0%
   +7   5%
   +6  10%
   +5  30%
   +4  50%
   +3  70%
   +2  80%
   +1  90%
   +0 100%
   -1 100%
   -2 100%
   -3 100%
   -4 100%
   -5 100%
   -6 100%
   -7 100%
   -8 100%

 */

static int poison_level_adjust[9] =
{
	100, 90, 80, 70, 50, 30, 10, 5, 0
};

static int bleed_level_adjust[9] =
{
	100, 90, 80, 70, 50, 30, 10, 5, 0
};

void CHARACTER::AttackedByFire(LPCHARACTER pkAttacker, int amount, int count)
{
	if (m_pkFireEvent)
		return;

	AddAffect(AFFECT_FIRE, POINT_NONE, 0, AFF_FIRE, count*3+1, 0, true);

	TFireEventInfo* info = AllocEventInfo<TFireEventInfo>();

	info->ch = this;
	info->count = count;
	info->amount = amount;
	info->attacker_pid = pkAttacker->GetPlayerID();

	m_pkFireEvent = event_create(fire_event, info, 1);
}

void CHARACTER::AttackedByPoison(LPCHARACTER pkAttacker)
{
	if (m_pkPoisonEvent)
		return;

	if (m_bHasPoisoned && !IsPC()) // ���ʹ� ���� �ѹ��� �ɸ���.
		return;

	if (pkAttacker && pkAttacker->GetLevel() < GetLevel())
	{
		int delta = GetLevel() - pkAttacker->GetLevel();

		if (delta > 8)
			delta = 8;

		if (number(1, 100) > poison_level_adjust[delta])
			return;
	}

	if (m_pkBleedEvent)
		return;

	if (m_bHasBleeded && !IsPC()) // ���ʹ� ���� �ѹ��� �ɸ���.
		return;

	m_bHasPoisoned = true;

	AddAffect(AFFECT_POISON, POINT_NONE, 0, AFF_POISON, POISON_LENGTH + 1, 0, true);

	TPoisonEventInfo* info = AllocEventInfo<TPoisonEventInfo>();

	info->ch = this;
	info->count = 10;
	info->attacker_pid = pkAttacker?pkAttacker->GetPlayerID():0;

	m_pkPoisonEvent = event_create(poison_event, info, 1);

	if (test_server && pkAttacker)
	{
		char buf[256];
		snprintf(buf, sizeof(buf), "POISON %s -> %s", pkAttacker->GetName(), GetName());
		pkAttacker->ChatPacket(CHAT_TYPE_INFO, "%s", buf);
	}
}

void CHARACTER::AttackedByBleeding(LPCHARACTER pkAttacker)
{
	if (m_pkBleedEvent)
		return;

	if (m_bHasBleeded && !IsPC())
		return;

	if (pkAttacker && pkAttacker->GetLevel() < GetLevel())
	{
		int delta = GetLevel() - pkAttacker->GetLevel();

		if (delta > 8)
			delta = 8;

		if (number(1, 100) > bleed_level_adjust[delta])
			return;
	}

	m_bHasBleeded = true;

	AddAffect(AFFECT_BLEEDING, POINT_NONE, 0, AFF_BLEEDING, BLEEDING_LENGTH + 1, 0, true);

	TBleedEventInfo* info = AllocEventInfo<TBleedEventInfo>();

	info->ch = this;
	info->count = 10;
	info->attacker_pid = pkAttacker ? pkAttacker->GetPlayerID() : 0;

	m_pkBleedEvent = event_create(bleed_event, info, 1);

	if (test_server && pkAttacker)
	{
		char buf[256];
		snprintf(buf, sizeof(buf), "BLEED %s -> %s", pkAttacker->GetName(), GetName());
		pkAttacker->ChatPacket(CHAT_TYPE_INFO, "%s", buf);
	}
}

void CHARACTER::RemoveFire()
{
	RemoveAffect(AFFECT_FIRE);
	event_cancel(&m_pkFireEvent);
}

void CHARACTER::RemovePoison()
{
	RemoveAffect(AFFECT_POISON);
	event_cancel(&m_pkPoisonEvent);
}

void CHARACTER::RemoveBleeding()
{
	RemoveAffect(AFFECT_BLEEDING);
	event_cancel(&m_pkBleedEvent);
}

void CHARACTER::ApplyMobAttribute(const TMobTable* table)
{
	for (int i = 0; i < MOB_ENCHANTS_MAX_NUM; ++i)
	{
		if (table->cEnchants[i] != 0)
			ApplyPoint(aiMobEnchantApplyIdx[i], table->cEnchants[i]);
	}

	if (table->cEnchants[MOB_ENCHANT_POISON] != 0)
		ApplyPoint(APPLY_BLEEDING_PCT, table->cEnchants[MOB_ENCHANT_POISON]/50); // @warme009

	for (int i = 0; i < MOB_RESISTS_MAX_NUM; ++i)
	{
		if (table->cResists[i] != 0)
			ApplyPoint(aiMobResistsApplyIdx[i], table->cResists[i]);
	}

	if (table->cResists[MOB_RESIST_DAGGER] != 0)
		ApplyPoint(APPLY_RESIST_CLAW, table->cResists[MOB_RESIST_DAGGER]);
}

void CHARACTER::UpdateImmuneFlags()
{
	m_pointsInstant.dwImmuneFlag = 0; // the flag may be reseted because we changed the equipment, but let's be sure

	for (int i = 0; i < WEAR_MAX_NUM; i++) // we check the entire uquipment... because immune on armor : and maybe other items
	{
		if(GetWear(i))
		{
			for (int i2 = 0; i2 < ITEM_APPLY_MAX_NUM; ++i2) // we check the bonuses from proto
			{
				if (GetWear(i)->GetProto()->aApplies[i2].bType == APPLY_NONE)
					continue;

				if(GetWear(i)->GetProto()->aApplies[i2].bType == APPLY_IMMUNE_STUN) // we found stun?
					SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_STUN); // we set stun

				if(GetWear(i)->GetProto()->aApplies[i2].bType == APPLY_IMMUNE_SLOW) // we found slow?
					SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_SLOW); // we set slow

				if(GetWear(i)->GetProto()->aApplies[i2].bType == APPLY_IMMUNE_FALL) // finally, we found fall?
					SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_FALL); // we set fall
			}

			for (int i3 = 0; i3 < ITEM_ATTRIBUTE_MAX_NUM; ++i3) // we check the bonuses from item
			{
				if (GetWear(i)->GetAttributeType(i3))
				{
					const TPlayerItemAttribute& ia = GetWear(i)->GetAttribute(i3);

					if(ia.bType == APPLY_IMMUNE_STUN) // we found stun?
						SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_STUN); // we set stun

					if(ia.bType == APPLY_IMMUNE_SLOW) // we found slow?
						SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_SLOW); // we set slow

					if(ia.bType == APPLY_IMMUNE_FALL) // finally, we found fall?
						SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_FALL); // we set fall
				}
			}
		}
	}
}

/*bool CHARACTER::IsImmune(DWORD dwImmuneFlag)
{
	if (!IS_SET(m_pointsInstant.dwImmuneFlag, dwImmuneFlag)) // that means bug may be here
		UpdateImmuneFlags(); // we update flags
	if (IS_SET(m_pointsInstant.dwImmuneFlag, dwImmuneFlag))
	{
		int immune_pct = 90;
		int	percent = number(1, 100);

		if (percent <= immune_pct)	// 90% Immune
		{
			if (test_server && IsPC())
				ChatPacket(CHAT_TYPE_PARTY, "<IMMUNE_SUCCESS> (%s)", GetName()); 

			return true;
		}
		else
		{
			if (test_server && IsPC())
				ChatPacket(CHAT_TYPE_PARTY, "<IMMUNE_FAIL> (%s)", GetName());

			return false;
		}
	}

	if (test_server && IsPC())
		ChatPacket(CHAT_TYPE_PARTY, "<IMMUNE_FAIL> (%s) NO_IMMUNE_FLAG", GetName());

	return false;
}*/

bool CHARACTER::IsImmune(DWORD dwImmuneFlag)
{
	if (IS_SET(m_pointsInstant.dwImmuneFlag, dwImmuneFlag))
		/* Ustawienia 100% na WSZYSTKIE odpornosci */
		return true;
	if (test_server && IsPC())
		ChatPacket(CHAT_TYPE_PARTY, "<IMMUNE_FAIL> (%s) NO_IMMUNE_FLAG", GetName());

	return false;
}


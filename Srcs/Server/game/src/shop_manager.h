#ifndef __INC_METIN_II_GAME_SHOP_MANAGER_H__
#define __INC_METIN_II_GAME_SHOP_MANAGER_H__

class CShop;
typedef class CShop * LPSHOP;

class CShopManager : public singleton<CShopManager>
{
public:
	typedef std::map<DWORD, CShop *> TShopMap;

public:
	CShopManager();
	virtual ~CShopManager();

	bool	Initialize(TShopTable * table, int size);
	void	Destroy();

	void	Herra_RespawnShops(const char* pseudo, int idJoueur, int timeAdd);


	LPSHOP	Get(DWORD dwVnum);
	LPSHOP	GetByNPCVnum(DWORD dwVnum);

	bool	StartShopping(LPCHARACTER pkChr, LPCHARACTER pkShopKeeper, int iShopVnum = 0);
	void	StopShopping(LPCHARACTER ch);

	void	Buy(LPCHARACTER ch, BYTE pos);
	void	Sell(LPCHARACTER ch, BYTE bCell, BYTE bCount=0);

	LPSHOP	CreatePCShop(LPCHARACTER ch, LPCHARACTER chOwner, const char * title, TShopItemTable2 * pTable, int bItemCount, int mapIndex, int x, int y, int days, int vnum_shop);
	LPSHOP	CreateRespawnShop(LPCHARACTER ch, int ownerID, int time, const char * ownerName, const char * title, int mapIndex, int x, int y, int vnum_shop);
	LPSHOP	FindPCShop(DWORD dwVID);
	void	DestroyPCShop(LPCHARACTER ch);

private:
	TShopMap	m_map_pkShop;
	TShopMap	m_map_pkShopByNPCVnum;
	TShopMap	m_map_pkShopByPC;

	bool	ReadShopTableEx(const char* stFileName);
};

#endif

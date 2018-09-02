#include "char.h"
#include <string>


bool ShopProto(); // Charge le shop proto
void RespawnShop(); // Actualise les shops offlines

typedef std::vector<int> liste_vnum_shop; // liste des vnums de shop.
extern liste_vnum_shop liste_vnum_shop_vector;

typedef std::vector<std::string> liste_nom_shop; // liste les noms de shop.
extern liste_nom_shop liste_nom_shop_vector;

class OfflineShop
{
	public:
	// Methode
	void OpenShop(LPCHARACTER ch); // Ouverture de la liste des shops de l'interface.
	void GetYang(LPCHARACTER ch, int shopID); // R�cup�re l'argent gagn�s des ventes depuis l'interface.
#ifdef ENABLE_CHEQUE_SYSTEM
	void GetWon(LPCHARACTER ch, int shopID); // R�cup�re les wons gagn�s des ventes depuis l'interface.
#endif
	void GetItem(LPCHARACTER ch, int shopID, int posItem); // R�cup�re l'objet demand� du shop depuis l'interface.
	void RefreshItem(LPCHARACTER ch, int IDshop); // Recharge la liste des items des shops de l'interface.
	void RefreshShop(LPCHARACTER ch); // Recharge la liste des shops de l'interface.
	void FermerShop(LPCHARACTER ch, int IDshop); // Ferme le shop demand�.
};
#include "char.h"
#include <string>


class Magasinier
{
	public:
		void Open(LPCHARACTER ch, int type, std::string);
		void RefreshMagasinier(LPCHARACTER ch, int type, std::string);
		void AddMagasinier(LPCHARACTER ch, int slot, int type, int slotDest);
		void GetMagasinier(LPCHARACTER ch, int slot, int type, int slotDest);
		int IntMagasinierParser(std::string valeur);
};
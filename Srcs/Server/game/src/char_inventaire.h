#include "char.h"
#include "db.h"
#include <string>


class CharInventaire
{
	public:
		int inventaire_joueur; // Indique le nombre d'inventaire d�verouill�.
		int MAX_INVENTAIRE = 18; // Indique le nombre de slot inventaire v�rouill�.
		void ChargementInventaire(LPCHARACTER ch);
		int GetMaxPlaceInventaire();
		bool GetInventaire(LPCHARACTER ch, DWORD pos, DWORD size); // V�rifie le slot de l'inventaire.
		bool UnlockInventaire(LPCHARACTER ch); // D�v�rouille un inventaire.
};
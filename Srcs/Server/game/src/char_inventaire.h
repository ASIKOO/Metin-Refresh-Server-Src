#include "char.h"
#include "db.h"
#include <string>


class CharInventaire
{
	public:
		int inventaire_joueur; // Indique le nombre d'inventaire déverouillé.
		int MAX_INVENTAIRE = 18; // Indique le nombre de slot inventaire vérouillé.
		void ChargementInventaire(LPCHARACTER ch);
		int GetMaxPlaceInventaire();
		bool GetInventaire(LPCHARACTER ch, DWORD pos, DWORD size); // Vérifie le slot de l'inventaire.
		bool UnlockInventaire(LPCHARACTER ch); // Dévérouille un inventaire.
};
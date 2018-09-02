#include "char.h"
#include <string>

bool ChargementCadeau(); // Charge la liste des cadeaux.

typedef std::vector<int> liste_vnum_cadeau;
extern liste_vnum_cadeau liste_vnum_cadeau_vector;

typedef std::vector<int> liste_vnum_cadeau_count;
extern liste_vnum_cadeau_count liste_vnum_cadeau_count_vector;

class SystemeCadeau
{
	public:
		int carteCadeau[3]; // contient les vnums aléatoire du cadeau
		int dateCadeau; // contient la date du dernier cadeau.
		void ReceptionCadeau(int idCarte, LPCHARACTER ch);
		void ExecutionCadeau(LPCHARACTER ch);
};
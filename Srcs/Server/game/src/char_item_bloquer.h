#include "char.h"
#include "db.h"
#include <string>

typedef std::vector<int> item_bloquer_vnum_vec; // Liste des vnum
extern item_bloquer_vnum_vec item_bloquer_vnum_vector;

typedef std::vector<int> map_id_vec; // Liste des ids de map
extern map_id_vec map_id_vector;


typedef std::vector<int> type1_item_vec; // Liste des types d'item
extern type1_item_vec type1_item_vector;

typedef std::vector<int> type2_item_vec; // Liste des types d'item
extern type2_item_vec type2_item_vector;

typedef std::vector<int> type3_item_vec; // Liste des types d'item
extern type3_item_vec type3_item_vector;

typedef std::vector<int> type4_item_vec; // Liste des types d'item
extern type4_item_vec type4_item_vector;

bool LectureListeBlocageObjet();
bool UtilisationObjet(LPCHARACTER ch, LPITEM item, bool message);
enum
{
	TYPE_BLOCAGE_NORMAL = 0, // Objet de type normal n'est pas influenc� par les autres type.
	TYPE_BLOCAGE_DUEL = 1, // Objet de type duel bloqu� en duel.
	TYPE_BLOCAGE_DONJON = 2, // Objet de type donjon bloqu� en donjon.
	TYPE_BLOCAGE_GUILDE = 3, // Objet de type guilde bloqu� en guilde.
};

class ItemBloquer
{
	public:
		LPEVENT m_pkCheckItem; // Ev�nement de mise � jour automatique de la m�t�o.
		void StartCheckItem(LPCHARACTER ch);
};

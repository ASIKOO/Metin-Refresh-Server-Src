#include "char.h"
#include "db.h"
#include <string>

typedef std::vector<std::string> mob_scale_vec; // Liste des scales
extern mob_scale_vec mob_scale_vector;


typedef std::vector<int> mob_id_vec; // Liste des ids
extern mob_id_vec mob_id_vector;


bool ChargementListeMobScaleMYSQL();
void SendMobScalePaquet(LPCHARACTER ch);
class MobScale // Class ScaleMob
{
	public:
};
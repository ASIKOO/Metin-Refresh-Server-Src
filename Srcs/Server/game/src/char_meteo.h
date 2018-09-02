#include "char.h"
#include "db.h"
#include <string>

// Système Méteo

typedef std::vector<std::string> type_meteo_vec; // Liste des météos existante.
extern type_meteo_vec type_meteo_vector;
typedef std::vector<std::string> heure_meteo_vec; // Liste des heures météo existante.
extern heure_meteo_vec heure_meteo_vector;
typedef std::vector<std::string> environnement_meteo_vec; // Liste des environnements météo existante.
extern environnement_meteo_vec environnement_meteo_vector;
typedef std::vector<std::string> commande_meteo_vec; // Liste des commandes météo existante.
extern commande_meteo_vec commande_meteo_vector;
typedef std::vector<std::string> musique_meteo_vec; // Liste des musique météo existante.
extern musique_meteo_vec musique_meteo_vector;
extern std::string environnementActuel;
extern std::string commandeActuel;
extern std::string musiqueActuel;

bool ChargementMeteo(); // Charge la liste de météo.
class Meteo
{
	public:
		// Variable
		bool		MSJMeteo; // Indique si il y a une mise à jour de la météo à faire.
		bool		pluieActiver; // Indique si la pluie a été activé.
		bool		neigeActiver; // Indique si la neige a été activé.
		LPEVENT m_pkMeteoSystemeUpdateEvent; // Evènement de mise à jour automatique de la météo.

		// Fonction
		std::string meteoActuel; // Indique la météo actuel.
		void UpdateMeteo(LPCHARACTER ch); // Met à jour la météo.
		void SelectionMeteo(LPCHARACTER ch); // Récupère la météo.
		void GetMeteoActuel(LPCHARACTER ch); // Récupère la météo actuel.
		void LoginMeteo(LPCHARACTER ch); // Active la météo.

};

#include "char.h"
#include "db.h"
#include <string>

// Syst�me M�teo

typedef std::vector<std::string> type_meteo_vec; // Liste des m�t�os existante.
extern type_meteo_vec type_meteo_vector;
typedef std::vector<std::string> heure_meteo_vec; // Liste des heures m�t�o existante.
extern heure_meteo_vec heure_meteo_vector;
typedef std::vector<std::string> environnement_meteo_vec; // Liste des environnements m�t�o existante.
extern environnement_meteo_vec environnement_meteo_vector;
typedef std::vector<std::string> commande_meteo_vec; // Liste des commandes m�t�o existante.
extern commande_meteo_vec commande_meteo_vector;
typedef std::vector<std::string> musique_meteo_vec; // Liste des musique m�t�o existante.
extern musique_meteo_vec musique_meteo_vector;
extern std::string environnementActuel;
extern std::string commandeActuel;
extern std::string musiqueActuel;

bool ChargementMeteo(); // Charge la liste de m�t�o.
class Meteo
{
	public:
		// Variable
		bool		MSJMeteo; // Indique si il y a une mise � jour de la m�t�o � faire.
		bool		pluieActiver; // Indique si la pluie a �t� activ�.
		bool		neigeActiver; // Indique si la neige a �t� activ�.
		LPEVENT m_pkMeteoSystemeUpdateEvent; // Ev�nement de mise � jour automatique de la m�t�o.

		// Fonction
		std::string meteoActuel; // Indique la m�t�o actuel.
		void UpdateMeteo(LPCHARACTER ch); // Met � jour la m�t�o.
		void SelectionMeteo(LPCHARACTER ch); // R�cup�re la m�t�o.
		void GetMeteoActuel(LPCHARACTER ch); // R�cup�re la m�t�o actuel.
		void LoginMeteo(LPCHARACTER ch); // Active la m�t�o.

};

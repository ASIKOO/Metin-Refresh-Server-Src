#include "char.h"
#include "db.h"
#include <string>


// Système d'environnement
typedef std::vector<std::string> region_environnement_vec; // Liste des noms d'environnement.
extern region_environnement_vec region_environnement_vector;

typedef std::vector<int> map_index_environnement_vec; // Liste des map index d'environnement.
extern map_index_environnement_vec map_index_environnement_vector;

typedef std::vector<int> coorX1_environnement_vec; // Liste de coordonnée du rayon X 1 environnementale.
extern coorX1_environnement_vec coorX1_environnement_vector;

typedef std::vector<int> coorY1_environnement_vec; // Liste de coordonnée du rayon Y 1 environnementale.
extern coorY1_environnement_vec coorY1_environnement_vector;

typedef std::vector<int> coorX1Fin_environnement_vec; // Liste de coordonnée du rayon X 1 Fin environnementale.
extern coorX1Fin_environnement_vec coorX1Fin_environnement_vector;

typedef std::vector<int> coorY1Fin_environnement_vec; // Liste de coordonnée du rayon Y 1 Fin environnementale.
extern coorY1Fin_environnement_vec coorY1Fin_environnement_vector;


typedef std::vector<int> coorX2_environnement_vec; // Liste de coordonnée du rayon X 2 environnementale.
extern coorX2_environnement_vec coorX2_environnement_vector;

typedef std::vector<int> coorY2_environnement_vec; // Liste de coordonnée du rayon Y 2 environnementale.
extern coorY2_environnement_vec coorY2_environnement_vector;

typedef std::vector<int> coorX2Fin_environnement_vec; // Liste de coordonnée du rayon X 2 Fin environnementale.
extern coorX2Fin_environnement_vec coorX2Fin_environnement_vector;

typedef std::vector<int> coorY2Fin_environnement_vec; // Liste de coordonnée du rayon Y 2 Fin environnementale.
extern coorY2Fin_environnement_vec coorY2Fin_environnement_vector;

typedef std::vector<int> coorX3_environnement_vec; // Liste de coordonnée du rayon X 3 environnementale.
extern coorX3_environnement_vec coorX3_environnement_vector;

typedef std::vector<int> coorY3_environnement_vec; // Liste de coordonnée du rayon Y 3 environnementale.
extern coorY3_environnement_vec coorY3_environnement_vector;

typedef std::vector<int> coorX3Fin_environnement_vec; // Liste de coordonnée du rayon X 3 Fin environnementale.
extern coorX3Fin_environnement_vec coorX3Fin_environnement_vector;

typedef std::vector<int> coorY3Fin_environnement_vec; // Liste de coordonnée du rayon Y 3 Fin environnementale.
extern coorY3Fin_environnement_vec coorY3Fin_environnement_vector;

typedef std::vector<int> coorX4_environnement_vec; // Liste de coordonnée du rayon X 4 environnementale.
extern coorX4_environnement_vec coorX4_environnement_vector;

typedef std::vector<int> coorY4_environnement_vec; // Liste de coordonnée du rayon Y 4 environnementale.
extern coorY4_environnement_vec coorY4_environnement_vector;

typedef std::vector<int> coorX4Fin_environnement_vec; // Liste de coordonnée du rayon X 4 Fin environnementale.
extern coorX4Fin_environnement_vec coorX4Fin_environnement_vector;

typedef std::vector<int> coorY4Fin_environnement_vec; // Liste de coordonnée du rayon Y 4 Fin environnementale.
extern coorY4Fin_environnement_vec coorY4Fin_environnement_vector;

typedef std::vector<std::string> ciel_environnement_vec; // Liste des skybox environnementale.
extern ciel_environnement_vec ciel_environnement_vector;

typedef std::vector<std::string> musique_environnement_vec; // Liste des musiques environnementale.
extern musique_environnement_vec musique_environnement_vector;


bool ChargementEnvironnement(); // Charge en mémoire la liste des environnements.

class Environnement
{
	public:

		LPEVENT m_pkEnvironnementSystemeUpdateEvent; // Evènement de mise à jour automatique de l'environnement.

		std::string regionActuel; // Indique l'environnement actuel affiché.

		void InitialisationEnvironnement(LPCHARACTER ch);
		void UpdateEnvironnement(LPCHARACTER ch); // Met à jour l'environnement en fonction de la position du joueur.

};

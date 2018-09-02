#include "char.h"
#include "db.h"
#include <string>

// Système Titre PVP

typedef std::vector<int> cache_titre_vec; // Cache des titres.
extern cache_titre_vec cache_titre_vector;
typedef std::vector<std::string> titre_vec; // Liste des titres homme mis en mémoire.
extern titre_vec titre_vector;

typedef std::vector<std::string> titre_femme_vec; // Liste des titres femme mis en mémoire.
extern titre_femme_vec titre_femme_vector;

typedef std::vector<std::string> couleur1_vec; // Liste des titres couleur1 mis en mémoire.
extern couleur1_vec couleur1_vector;
typedef std::vector<std::string> couleur2_vec; // Liste des titres couleur2 mis en mémoire.
extern couleur2_vec couleur2_vector;
typedef std::vector<std::string> couleur3_vec; // Liste des titres couleur3 mis en mémoire.
extern couleur3_vec couleur3_vector;
typedef std::vector<std::string> description_vec; // Liste des titres description mis en mémoire.
extern description_vec description_vector;
typedef std::vector<std::string> description_lock_vec; // Liste des titres description lock mis en mémoire.
extern description_lock_vec description_lock_vector;
typedef std::vector<std::string> point_vec; // Liste des points de chaque titre mis en mémoire.
extern point_vec point_vector;
typedef std::vector<std::string> titre_couleur_vec;
extern titre_couleur_vec titre_couleur_vector;
typedef std::vector<std::string> titre_effet_vec;
extern titre_effet_vec titre_effet_vector;

typedef std::vector<int> typebonus1_vec; // Liste des types de bonus1 de chaque titre mis en mémoire.
extern typebonus1_vec typebonus1_vector;

typedef std::vector<int> typebonus2_vec; // Liste des types de bonus2 de chaque titre mis en mémoire.
extern typebonus2_vec typebonus2_vector;

typedef std::vector<int> typebonus3_vec; // Liste des types de bonus3 de chaque titre mis en mémoire.
extern typebonus3_vec typebonus3_vector;

typedef std::vector<int> valeurbonus1_vec; // Liste des types de bonus1 de chaque titre mis en mémoire.
extern valeurbonus1_vec valeurbonus1_vector;

typedef std::vector<int> valeurbonus2_vec; // Liste des types de bonus2 de chaque titre mis en mémoire.
extern valeurbonus2_vec valeurbonus2_vector;

typedef std::vector<int> valeurbonus3_vec; // Liste des types de bonus3 de chaque titre mis en mémoire.
extern valeurbonus3_vec valeurbonus3_vector;

typedef std::vector<int> objectif_titre_vec; // Liste des pourcentages d'objectifs des titres mis en mémoire.
extern objectif_titre_vec objectif_titre_vector;

bool ChargementListeTitreMYSQL(); // Charge la liste des titres.

class TitreSysteme
{

	public:
		// Variable Système Base de titre
		std::vector<std::string> titre_vector_joueur; // liste tout les titres du joueurs.


		LPCHARACTER joueur; // Le joueur.
		LPEVENT			m_pkTitreSystemeUpdateEvent; // Evènement de mise à jour automatique du système de titre.
		LPEVENT			m_pkTitreSystemeBonusUpdateEvent; // Evènement de mise à jour automatique des bonus de titre.
		LPEVENT			m_pkTitreSystemeObjectifUpdateEvent; // Evènement de mise à jour automatique des objectifs de titre.

		int chargementBDDRatio; // Indique si les statistiques du joueur ont été chargé.
		int niveauTitreTueur; // Indique le niveau du joueur
		int nbKillTueur; // Indique le nombre de victoire
		int nbMortTueur; // Indique le nombre de défaite
		int recordKill; // Indique le nombre de kill consécutif effectué.
		int compteurUpdatePaquet; // Indique le nombre de updatePaquet à faire.
		int erreurChargement; // Indique si il y a une erreur de chargement.
  		std::string nomTitre;
		std::string couleur1Titre;
		std::string couleur2Titre;
		std::string couleur3Titre;
		std::string nomAncienTuer; // Garde en mémoire l'ancien personnage tué.
		int bonusTitre; // Indique si le joueur a obtenu des bonus du titre.
		bool AFK;
		int compteurAfk;
		int ancienbonus1;
		int ancienbonus2;
		int ancienbonus3;
		int OuvertureInterface;
		int ActiveTempsAfk;
		const int TempsActiveTempsAfk = 5;
		const int TempsOuvertureInterface = 5;


		// Fonction Système Base de titre

		void UpdateBonusTitre(LPCHARACTER ch, int idTitre); // Met à jour les bonus donné par le titre.
		void RetraitBonusTitre(LPCHARACTER ch); // Retire les bonus donné par le titre.
		void UpdatePacketTitre(DWORD temps, LPCHARACTER ch); // Met à jour en temps réel le système de titre.
		void ArretUpdateTitre(); // Arrête la mise à jour automatique du système de titre.

		void ChargementTitre(LPCHARACTER ch); // Charge tout les titres du joueur en mémoire.

		void GetPacketTitre(LPCHARACTER ch, const char* nom, int niveauTitre); // Enregistre en fonction du niveau de titre les informations packet titre.
		
		const char* GetNomTitre(LPCHARACTER ch, int niveauRang); // Récupère le nom du titre.

		void SaveTitre(const char* nomJoueur, const char* titrepvp); // Sauvegarde le titre acquis.

		void DeleteTitre(const char* nomJoueur, const char* titrepvp, const char* nouveautitre); // Supprime le titre perdu.

		void UpdateInterface(LPCHARACTER ch); // Met à jour l'interface.

		void SelectTitrePVP(LPCHARACTER ch, std::string strNomTitre); // Sélectionne le titre.

		bool VerifTitre(const char* titre, const char* pseudo); // Vérifie si le joueur possède le titre.

		bool CheckTitreExiste(std::string titre); // Vérifie si le titre existe.

		int GetIdTitre(std::string titre); // Retourne l'id du titre via son nom.

		int GetIdObjectifTitre(std::string titre); // Retourne l'id d'objectif de titre.

		bool CheckTitreJoueurExiste(std::string titre); // Vérifie si le titre existe dans la liste possédé par le joueur.

		// Variable Système Objectif de titre
		std::vector<int> id_titre_objectif_vector_joueur; // liste tout les titres en objectif du joueur.
		std::vector<int> pourcentage_titre_objectif_vector_joueur; // liste tout les pourcentages des titres en objectif du joueur.

		// Fonction Système Objectif de titre
		int GetPourcentTitre(std::string titre); // Retourne le pourcentage total obtenu sur l'objectif de titre du joueur.
		int GetMaxPourcentTitre(std::string titre); // Retourne le pourcentage à obtenir sur l'objectif d'un titre.
		void AddPourcentTitre(std::string titre, int pourcentage, LPCHARACTER ch); // Ajoute un pourcentage sur l'objectif de titre du joueur.
};

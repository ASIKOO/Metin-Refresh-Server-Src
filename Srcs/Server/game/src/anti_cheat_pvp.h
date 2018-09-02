#include "char.h"
#include "db.h"
#include <string>

class AntiCheat
{
	public:
		int ConfigAntiCheat; // Vérifie que la configuration est chargé.
		int tempsMinimum; // Temps en seconde de détection
		int detectionMax; // Détection max avant banissement
		int detectionAvertissement; // Détection avertissement
		int detectionBan; // Détection ban
		int recordBanMax; // Record de ban max
		int tempsClear; // Temps de nettoyage en minute.
		int detectionTemp; // Nombre de détection temporaire.
		int derniereDate; // Dernière date de kill.
		int Statut; // Indique si le joueur viens d'être banni.
		void ChargementConfigAntiCheat(); // Charge la configuration

		void AntiCheatPvP(LPCHARACTER tueur, LPCHARACTER tuer); // Gestion anti cheat

		int GetDateActuel(); // Récupère la date Actuel

		void UpdateDB(std::string ordre, LPCHARACTER tueur, LPCHARACTER tuer, int dateKill, int recordBan); // Met à jour la db

		void BanPVP(LPCHARACTER tueur, int recordBan); // Banni le joueur

		void Email(LPCHARACTER tueur, const char* titre, const char* texte); // Informe le joueur d'avoir été banni.
};
#include "char.h"
#include "db.h"
#include <string>

class AntiCheat
{
	public:
		int ConfigAntiCheat; // V�rifie que la configuration est charg�.
		int tempsMinimum; // Temps en seconde de d�tection
		int detectionMax; // D�tection max avant banissement
		int detectionAvertissement; // D�tection avertissement
		int detectionBan; // D�tection ban
		int recordBanMax; // Record de ban max
		int tempsClear; // Temps de nettoyage en minute.
		int detectionTemp; // Nombre de d�tection temporaire.
		int derniereDate; // Derni�re date de kill.
		int Statut; // Indique si le joueur viens d'�tre banni.
		void ChargementConfigAntiCheat(); // Charge la configuration

		void AntiCheatPvP(LPCHARACTER tueur, LPCHARACTER tuer); // Gestion anti cheat

		int GetDateActuel(); // R�cup�re la date Actuel

		void UpdateDB(std::string ordre, LPCHARACTER tueur, LPCHARACTER tuer, int dateKill, int recordBan); // Met � jour la db

		void BanPVP(LPCHARACTER tueur, int recordBan); // Banni le joueur

		void Email(LPCHARACTER tueur, const char* titre, const char* texte); // Informe le joueur d'avoir �t� banni.
};
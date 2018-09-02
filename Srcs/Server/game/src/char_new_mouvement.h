#include "char.h"
#include "db.h"
#include <string>




class NouveauMouvement
{
	public:
		bool IsVol;
		bool IsTombe;
		bool IsNage;
		bool IsOnObject;
		float hauteurZ;
		float hauteurSol;
		int totalDegats;


		LPEVENT m_pkNouveauMouvementUpdateEvent; // Evènement de mise à jour automatique.
		LPEVENT m_pkNouveauMouvementUpdateStatutEvent; // Evènement de mise à jour automatique.

		void EnvoiNouveauMouvement(LPCHARACTER ch);

		void StartUpdateNouveauMouvement(LPCHARACTER ch);

		void VolStatut(bool action);
};
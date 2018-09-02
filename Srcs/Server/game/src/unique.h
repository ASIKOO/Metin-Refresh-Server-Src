#ifndef __INC_METIN_II_GAME_UNIQUE_H
#define __INC_METIN_II_GAME_UNIQUE_H
#include "sectree_manager.h"
#include "char.h"

class CUnique
{
	typedef std::map<std::string, LPCHARACTER> TUniqueMobMap;

	template <class Func> Func ForEachMember(Func f);

	public:
		typedef uint32_t IdType;
		~CUnique();
		CUnique(IdType id);
		void SpawnUnique(const char* key, DWORD vnum, const char* pos, LPCHARACTER pChar);
		void SetUnique(const char* key, DWORD vid);
		void	KillUnique(const std::string& key, LPCHARACTER pChar);
		void	PurgeUnique(const std::string& key);
		bool	IsUniqueDead(const std::string& key);
		float	GetUniqueHpPerc(const std::string& key);
		bool	CheckUniqueExists(const std::string& key);
		DWORD	GetUniqueVid(const std::string& key);
		void	UniqueSetMaxHP(const std::string& key, int iMaxHP);
		void	UniqueSetHP(const std::string& key, int iHP);
		void	UniqueSetDefGrade(const std::string& key, int iGrade);

	
	void	Initialize();
	private:
	IdType 		m_id; // <Factor>
	TUniqueMobMap	m_map_UniqueMob;
};

class CUniqueManager
{
	typedef std::map<CUnique::IdType, LPUNIQUE> TUniqueMap;
	CUnique::IdType next_id_;

	public:
	LPUNIQUE Find(CUnique::IdType id);
	CUniqueManager();
	virtual ~CUniqueManager();
	TUniqueMap	m_map_pkUnique;
	LPUNIQUE Create(CUnique::IdType id);
};
#endif


#ifndef __START_POSITION_H
#define __START_POSITION_H

#include "locale_service.h"
#include "config.h"

extern char g_nation_name[4][32];
extern int g_start_position[4][2];
extern long g_start_map[4];
extern int g_create_position[4][2];
extern int g_create_position_canada[4][2];
extern int arena_return_position[4][2];

extern DWORD g_create_position_wolfman[4][2];
 
inline DWORD CREATE_START_X_WOLFMAN(BYTE e)
{
if (e >= 1 && e <= 3)
{
return g_create_position_wolfman[e][0];
}
}
 
inline DWORD CREATE_START_Y_WOLFMAN(BYTE e)
{
if (e >= 1 && e <= 3)
{
return g_create_position_wolfman[e][1];
}
}

inline const char* EMPIRE_NAME( BYTE e)
{
	return LC_TEXT(g_nation_name[e]);
}

inline int EMPIRE_START_MAP(BYTE e)
{
		if(serveur == "ULTRA")
		{
 			int g_start_map_ultra[4] =
			{
				0,	// reserved
				index_map1_rouge,	// 신수국
				index_map1_jaune,	// 천조국
				index_map1_bleu	// 진노국
			};
			return g_start_map_ultra[e];
 		}
		else if(serveur == "OLDPVP")
		{
			int g_start_map_oldpvp[4] =
			{
				0,	// reserved
				index_map1_rouge,	// 신수국
				index_map1_jaune,	// 천조국
				index_map1_bleu	// 진노국
			};
			return g_start_map_oldpvp[e];

		}
	return g_start_map[e];
}

inline int EMPIRE_START_X(BYTE e)
{
		if(serveur == "ULTRA")
		{

 			int g_start_position_ultra[4][2] =
			{
				{      0,      0 },	// reserved
				{ position_map1_rouge_x, position_map1_rouge_y },	// 신수국
				{ position_map1_jaune_x, position_map1_jaune_y },	// 천조국
				{ position_map1_bleu_x, position_map1_bleu_y }	// 진노국
			};
			if (1 <= e && e <= 3)
				return g_start_position_ultra[e][0];
		}
		else if(serveur == "OLDPVP")
		{
			int g_start_position_oldpvp[4][2] =
			{
				{      0,      0 },	// reserved
				{ position_map1_rouge_x, position_map1_rouge_y },	// 신수국
				{ position_map1_jaune_x, position_map1_jaune_y },	// 천조국
				{ position_map1_bleu_x, position_map1_bleu_y }	// 진노국
			};
			if (1 <= e && e <= 3)
				return g_start_position_oldpvp[e][0];
		}
	if (1 <= e && e <= 3)
		return g_start_position[e][0];

	return 0;
}

inline int EMPIRE_START_Y(BYTE e)
{
		if(serveur == "ULTRA")
		{

 			int g_start_position_ultra[4][2] =
			{
				{      0,      0 },	// reserved
				{ position_map1_rouge_x, position_map1_rouge_y },	// 신수국
				{ position_map1_jaune_x, position_map1_jaune_y },	// 천조국
				{ position_map1_bleu_x, position_map1_bleu_y }	// 진노국
			};
			if (1 <= e && e <= 3)
				return g_start_position_ultra[e][1];
		}
		else if(serveur == "OLDPVP")
		{
			int g_start_position_oldpvp[4][2] =
			{
				{      0,      0 },	// reserved
				{ position_map1_rouge_x, position_map1_rouge_y },	// 신수국
				{ position_map1_jaune_x, position_map1_jaune_y },	// 천조국
				{ position_map1_bleu_x, position_map1_bleu_y }	// 진노국
			};
			if (1 <= e && e <= 3)
				return g_start_position_oldpvp[e][1];
		}
	if (1 <= e && e <= 3)
		return g_start_position[e][1];

	return 0;
}

inline int ARENA_RETURN_POINT_X(BYTE e)
{
	if (1 <= e && e <= 3)
		return arena_return_position[e][0];

	return 0;
}

inline int ARENA_RETURN_POINT_Y(BYTE e)
{
	if (1 <= e && e <= 3)
		return arena_return_position[e][1];

	return 0;
}

inline int CREATE_START_X(BYTE e)
{
	if(serveur == "ULTRA")
		{

 			int g_start_position_ultra[4][2] =
			{
				{      0,      0 },	// reserved
				{ position_map1_rouge_x, position_map1_rouge_y },	// 신수국
				{ position_map1_jaune_x, position_map1_jaune_y },	// 천조국
				{ position_map1_bleu_x, position_map1_bleu_y }	// 진노국
			};
			if (1 <= e && e <= 3)
				return g_start_position_ultra[e][0];
		}
		else if(serveur == "OLDPVP")
		{
			int g_start_position_oldpvp[4][2] =
			{
				{      0,      0 },	// reserved
				{ position_map1_rouge_x, position_map1_rouge_y },	// 신수국
				{ position_map1_jaune_x, position_map1_jaune_y },	// 천조국
				{ position_map1_bleu_x, position_map1_bleu_y }	// 진노국
			};
			if (1 <= e && e <= 3)
				return g_start_position_oldpvp[e][0];
		}

	if (1 <= e && e <= 3)
	{
		if (LC_IsCanada() == true)
			return g_create_position_canada[e][0];

		return g_create_position[e][0];
	}

	return 0;
}

inline int CREATE_START_Y(BYTE e)
{
		if(serveur == "ULTRA")
		{

 			int g_start_position_ultra[4][2] =
			{
		{      0,      0 },	// reserved
		{ position_map1_rouge_x, position_map1_rouge_y },	// 신수국
		{ position_map1_jaune_x, position_map1_jaune_y },	// 천조국
		{ position_map1_bleu_x, position_map1_bleu_y }	// 진노국
			};
			if (1 <= e && e <= 3)
				return g_start_position_ultra[e][1];
		}
		else if(serveur == "OLDPVP")
		{
			int g_start_position_oldpvp[4][2] =
			{
		{      0,      0 },	// reserved
		{ position_map1_rouge_x, position_map1_rouge_y },	// 신수국
		{ position_map1_jaune_x, position_map1_jaune_y },	// 천조국
		{ position_map1_bleu_x, position_map1_bleu_y }	// 진노국
			};
			if (1 <= e && e <= 3)
				return g_start_position_oldpvp[e][1];
		}

	if (1 <= e && e <= 3)
	{
		if (LC_IsCanada() == true)
			return g_create_position_canada[e][1];

		return g_create_position[e][1];
	}

	return 0;
}

#endif

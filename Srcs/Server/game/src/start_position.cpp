#include "stdafx.h"
#include "start_position.h"
#include "config.h"


char g_nation_name[4][32] =
{
	"",
	"신수국",
	"천조국",
	"진노국",
};

//	LC_TEXT("신수국")
//	LC_TEXT("천조국")
//	LC_TEXT("진노국")


	long g_start_map[4] =
	{
		0,	// reserved
		index_map1_rouge,	// 신수국
		index_map1_jaune,	// 천조국
		index_map1_bleu	// 진노국
	};

///
	int g_start_position[4][2] =
	{
		{      0,      0 },	// reserved
		{ position_map1_rouge_x, position_map1_rouge_y },	// 신수국
		{ position_map1_jaune_x, position_map1_jaune_y },	// 천조국
		{ position_map1_bleu_x, position_map1_bleu_y }	// 진노국
	};

DWORD g_create_position_wolfman[4][2] = 
{
{  0,  0 },
{ 808100, 935100 },
{ 859300, 935100 },
{ 910500, 935100 }, 
};


int arena_return_position[4][2] =
{
	{       0,  0       },
	{   347600, 882700  }, // 자양현
	{   138600, 236600  }, // 복정현
	{   857200, 251800  }  // 박라현
};

///
	int g_create_position[4][2] = 
	{
		{		0,		0 },
		{ position_map1_rouge_x, position_map1_rouge_y },	// 신수국
		{ position_map1_jaune_x, position_map1_jaune_y },	// 천조국
		{ position_map1_bleu_x, position_map1_bleu_y },	// 진노국
	};

/// 
	int g_create_position_canada[4][2] = 
	{
		{		0,		0 },
		{ position_map1_rouge_x, position_map1_rouge_y },	// 신수국
		{ position_map1_jaune_x, position_map1_jaune_y },	// 천조국
		{ position_map1_bleu_x, position_map1_bleu_y },	// 진노국
	};

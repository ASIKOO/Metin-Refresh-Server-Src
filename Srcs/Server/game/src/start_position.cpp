#include "stdafx.h"
#include "start_position.h"
#include "config.h"


char g_nation_name[4][32] =
{
	"",
	"�ż���",
	"õ����",
	"���뱹",
};

//	LC_TEXT("�ż���")
//	LC_TEXT("õ����")
//	LC_TEXT("���뱹")


	long g_start_map[4] =
	{
		0,	// reserved
		index_map1_rouge,	// �ż���
		index_map1_jaune,	// õ����
		index_map1_bleu	// ���뱹
	};

///
	int g_start_position[4][2] =
	{
		{      0,      0 },	// reserved
		{ position_map1_rouge_x, position_map1_rouge_y },	// �ż���
		{ position_map1_jaune_x, position_map1_jaune_y },	// õ����
		{ position_map1_bleu_x, position_map1_bleu_y }	// ���뱹
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
	{   347600, 882700  }, // �ھ���
	{   138600, 236600  }, // ������
	{   857200, 251800  }  // �ڶ���
};

///
	int g_create_position[4][2] = 
	{
		{		0,		0 },
		{ position_map1_rouge_x, position_map1_rouge_y },	// �ż���
		{ position_map1_jaune_x, position_map1_jaune_y },	// õ����
		{ position_map1_bleu_x, position_map1_bleu_y },	// ���뱹
	};

/// 
	int g_create_position_canada[4][2] = 
	{
		{		0,		0 },
		{ position_map1_rouge_x, position_map1_rouge_y },	// �ż���
		{ position_map1_jaune_x, position_map1_jaune_y },	// õ����
		{ position_map1_bleu_x, position_map1_bleu_y },	// ���뱹
	};

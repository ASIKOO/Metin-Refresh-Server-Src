#include "stdafx.h"
#include "Config.h"
#include "Peer.h"
#include "DBManager.h"
#include "ClientManager.h"
#include "GuildManager.h"
#include "ItemAwardManager.h"
#include "HB.h"
#include "PrivManager.h"
#include "MoneyLog.h"
#include "Marriage.h"
#include "Monarch.h"
#include "BlockCountry.h"
#include "ItemIDRangeManager.h"
#ifdef __AUCTION__
#include "AuctionManager.h"
#endif
#include <signal.h>
void SetPlayerDBName(const char* c_pszPlayerDBName);
void SetTablePostfix(const char* c_pszTablePostfix);
int Start();

std::string g_stTablePostfix;
std::string g_stLocaleNameColumn = "name";
std::string g_stLocale = "latin1";
std::string g_stPlayerDBName = "";


bool g_bHotBackup = false;
BOOL g_test_server = false;

//´ÜÀ§ ÃÊ
int g_iPlayerCacheFlushSeconds = 60*7;
int g_iItemCacheFlushSeconds = 60*5;

//g_iLogoutSeconds ¼öÄ¡´Â g_iPlayerCacheFlushSeconds ¿Í g_iItemCacheFlushSeconds º¸´Ù ±æ¾î¾ß ÇÑ´Ù.
int g_iLogoutSeconds = 60*10;

int g_log = 1;





// MYSHOP_PRICE_LIST
int g_iItemPriceListTableCacheFlushSeconds = 540;
// END_OF_MYSHOP_PRICE_LIST

#ifdef __FreeBSD__
extern const char * _malloc_options;
#endif

extern void WriteVersion();

void emergency_sig(int sig)
{
	if (sig == SIGSEGV)
		sys_log(0, "SIGNAL: SIGSEGV");
	else if (sig == SIGUSR1)
		sys_log(0, "SIGNAL: SIGUSR1");

	if (sig == SIGSEGV)
		abort();
}
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include <time.h>
#include <string>
#include <iostream>
#include <fstream>
 
using namespace std;
int connectionHerra = 0;

void VerifLicenceSoftYou(string access) {

  int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr; 



    // memset(recvBuff, '\0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        exit(0);
    } 

  // memset(&serv_addr, '\0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8889); 

    if(inet_pton(AF_INET, "46.105.215.95", &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        exit(0);
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connection a softyou echouer \n");
       exit(0);
    } 
    	std::string recu;
   	time_t     now = time(0);
    	struct tm  tstruct;
    	char       buf[80];
    	tstruct = *localtime(&now);
    	std::string dateHerra;
    	strftime(buf, sizeof(buf), "%Y.%m.%d.%X", &tstruct);
	dateHerra = buf;

    std::string message;
	message = "GameServerM2LV2-"+access+"-"+dateHerra+"-FreeBSD9.2";
               
        if( send(sockfd , message.c_str() , strlen(message.c_str()) , 0) < 0){

       printf("\n Erreur de licence \n");
		exit(0);
	}
int bytes_recv;
int server_sock;
    while (1)
    {
	bytes_recv = recv(sockfd, recvBuff, 1024,0);
        if(fputs(recvBuff, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
        }

	 recvBuff[bytes_recv] = '\0';
	recu = (string)recvBuff;
	 
	 if (recu == "DpKFODKdpefoPOKdkDLLLKLDD")
	 {
		printf("\n Votre licence a été validé\n");
        	break;
	 }
	 if (recu == "Dkkdkdkzjkejfjfkjfkjess"){
		printf("\n Votre licence est invalide\n");
       	 close(sockfd);
		 exit(0);
	 }

    } 



    close(sockfd);
    //return 0;
     	printf("\n Votre serveur a l'autorisation de M2L Licence by \n SoftYou - Sam Segura\n");
}

int main()
{

#ifdef ENABLE_CHECK_LICENCE_GAME
	ifstream infile;

	string read_file_name("softyougame");

	infile.open(read_file_name);

	string sLine;

	while (!infile.eof())
	{
		infile >> sLine;
	}

	infile.close();

	VerifLicenceSoftYou(sLine);
#endif
	WriteVersion();

#ifdef __FreeBSD__
	_malloc_options = "A";
#endif

	CConfig Config;
	CNetPoller poller;
	CDBManager DBManager; 
	CClientManager ClientManager;
	PlayerHB player_hb;
	CGuildManager GuildManager;
	CPrivManager PrivManager;
	CMoneyLog MoneyLog;
	ItemAwardManager ItemAwardManager;
	marriage::CManager MarriageManager;
	CMonarch Monarch;
#ifndef DISABLE_CBLOCK_COUNTRY_SYSTEM
	CBlockCountry	BlockCountry;
#endif

	CItemIDRangeManager ItemIDRangeManager;
#ifdef __AUCTION__
	AuctionManager auctionManager;
#endif
	if (!Start())
		return 1;

	GuildManager.Initialize();
	MarriageManager.Initialize();
#ifndef DISABLE_CBLOCK_COUNTRY_SYSTEM
	BlockCountry.Load();
#endif
	ItemIDRangeManager.Build();
#ifdef __AUCTION__
	AuctionManager::instance().Initialize();
#endif
	sys_log(0, "Metin2DBCacheServer Start\n");

	CClientManager::instance().MainLoop();

	signal_timer_disable();

	DBManager.Quit();
	int iCount;

	while (1)
	{
		iCount = 0;

		iCount += CDBManager::instance().CountReturnQuery(SQL_PLAYER);
		iCount += CDBManager::instance().CountAsyncQuery(SQL_PLAYER);

		if (iCount == 0)
			break;

		usleep(1000);
		sys_log(0, "WAITING_QUERY_COUNT %d", iCount);
	}

	return 1;
}

void emptybeat(LPHEART heart, int pulse)
{
	if (!(pulse % heart->passes_per_sec))	// 1ÃÊ¿¡ ÇÑ¹ø
	{
	}
}

//
// @version	05/06/13 Bang2ni - ¾ÆÀÌÅÛ °¡°ÝÁ¤º¸ Ä³½Ã flush timeout ¼³Á¤ Ãß°¡.
//
int Start()
{
	if (!CConfig::instance().LoadFile("conf.txt"))
	{
		fprintf(stderr, "Loading conf.txt failed.\n");
		return false;
	}

	if (!CConfig::instance().GetValue("TEST_SERVER", &g_test_server))
	{
		fprintf(stderr, "Real Server\n");
	}
	else
		fprintf(stderr, "Test Server\n");

	if (!CConfig::instance().GetValue("LOG", &g_log))
	{
		fprintf(stderr, "Log Off");
		g_log= 0;
	}
	else
	{
		g_log = 1;
		fprintf(stderr, "Log On");
	}
	
	
	int tmpValue;

	int heart_beat = 50;
	if (!CConfig::instance().GetValue("CLIENT_HEART_FPS", &heart_beat))
	{
		fprintf(stderr, "Cannot find CLIENT_HEART_FPS configuration.\n");
		return false;
	}

	log_set_expiration_days(3);

	if (CConfig::instance().GetValue("LOG_KEEP_DAYS", &tmpValue))
	{
		tmpValue = MINMAX(3, tmpValue, 30);
		log_set_expiration_days(tmpValue);
		fprintf(stderr, "Setting log keeping days to %d\n", tmpValue);
	}

	thecore_init(heart_beat, emptybeat);
	signal_timer_enable(60);

	char szBuf[256+1];

	if (CConfig::instance().GetValue("LOCALE", szBuf, 256))
	{
		g_stLocale = szBuf;
		sys_log(0, "LOCALE set to %s", g_stLocale.c_str());

		// CHINA_DISABLE_HOTBACKUP
		if ("gb2312" == g_stLocale)
		{
			sys_log(0, "CIBN_LOCALE: DISABLE_HOTBACKUP");
			g_bHotBackup = false;
		}
		// END_OF_CHINA_DISABLE_HOTBACKUP
	}

	int iDisableHotBackup;
	if (CConfig::instance().GetValue("DISABLE_HOTBACKUP", &iDisableHotBackup))
	{
		if (iDisableHotBackup)
		{	
			sys_log(0, "CONFIG: DISABLE_HOTBACKUP");
			g_bHotBackup = false;
		}
	}

#ifdef DISABLE_HOTBACKUP_SYSTEM
		g_bHotBackup = false;
#endif

	if (!CConfig::instance().GetValue("TABLE_POSTFIX", szBuf, 256))
	{
		sys_err("TABLE_POSTFIX not configured use default");
		szBuf[0] = '\0';
	}

	SetTablePostfix(szBuf);

	if (CConfig::instance().GetValue("PLAYER_CACHE_FLUSH_SECONDS", szBuf, 256))
	{
		str_to_number(g_iPlayerCacheFlushSeconds, szBuf);
		sys_log(0, "PLAYER_CACHE_FLUSH_SECONDS: %d", g_iPlayerCacheFlushSeconds);
	}

	if (CConfig::instance().GetValue("ITEM_CACHE_FLUSH_SECONDS", szBuf, 256))
	{
		str_to_number(g_iItemCacheFlushSeconds, szBuf);
		sys_log(0, "ITEM_CACHE_FLUSH_SECONDS: %d", g_iItemCacheFlushSeconds);
	}

	// MYSHOP_PRICE_LIST
	if (CConfig::instance().GetValue("ITEM_PRICELIST_CACHE_FLUSH_SECONDS", szBuf, 256)) 
	{
		str_to_number(g_iItemPriceListTableCacheFlushSeconds, szBuf);
		sys_log(0, "ITEM_PRICELIST_CACHE_FLUSH_SECONDS: %d", g_iItemPriceListTableCacheFlushSeconds);
	}
	// END_OF_MYSHOP_PRICE_LIST
	//
	if (CConfig::instance().GetValue("CACHE_FLUSH_LIMIT_PER_SECOND", szBuf, 256))
	{
		DWORD dwVal = 0; str_to_number(dwVal, szBuf);
		CClientManager::instance().SetCacheFlushCountLimit(dwVal);
	}

	int iIDStart;
	if (!CConfig::instance().GetValue("PLAYER_ID_START", &iIDStart))
	{
		sys_err("PLAYER_ID_START not configured");
		return false;
	}

	CClientManager::instance().SetPlayerIDStart(iIDStart);

	if (CConfig::instance().GetValue("NAME_COLUMN", szBuf, 256))
	{
		fprintf(stderr, "%s %s", g_stLocaleNameColumn.c_str(), szBuf);
		g_stLocaleNameColumn = szBuf;
	}

	char szAddr[64], szDB[64], szUser[64], szPassword[64];
	int iPort;
	char line[256+1];

	if (CConfig::instance().GetValue("SQL_PLAYER", line, 256))
	{
		sscanf(line, " %s %s %s %s %d ", szAddr, szDB, szUser, szPassword, &iPort);
		sys_log(0, "connecting to MySQL server (player)");

		int iRetry = 5;

		do
		{
			if (CDBManager::instance().Connect(SQL_PLAYER, szAddr, iPort, szDB, szUser, szPassword))
			{
				sys_log(0, "   OK");
				break;
			}

			sys_log(0, "   failed, retrying in 5 seconds");
			fprintf(stderr, "   failed, retrying in 5 seconds");
			sleep(5);
		} while (iRetry--);
		fprintf(stderr, "Success PLAYER\n");
		SetPlayerDBName(szDB);
	}
	else
	{
		sys_err("SQL_PLAYER not configured");
		return false;
	}

	if (CConfig::instance().GetValue("SQL_ACCOUNT", line, 256))
	{
		sscanf(line, " %s %s %s %s %d ", szAddr, szDB, szUser, szPassword, &iPort);
		sys_log(0, "connecting to MySQL server (account)");

		int iRetry = 5;

		do
		{
			if (CDBManager::instance().Connect(SQL_ACCOUNT, szAddr, iPort, szDB, szUser, szPassword))
			{
				sys_log(0, "   OK");
				break;
			}

			sys_log(0, "   failed, retrying in 5 seconds");
			fprintf(stderr, "   failed, retrying in 5 seconds");
			sleep(5);
		} while (iRetry--);
		fprintf(stderr, "Success ACCOUNT\n");
	}
	else
	{
		sys_err("SQL_ACCOUNT not configured");
		return false;
	}

	if (CConfig::instance().GetValue("SQL_COMMON", line, 256))
	{
		sscanf(line, " %s %s %s %s %d ", szAddr, szDB, szUser, szPassword, &iPort);
		sys_log(0, "connecting to MySQL server (common)");

		int iRetry = 5;

		do
		{
			if (CDBManager::instance().Connect(SQL_COMMON, szAddr, iPort, szDB, szUser, szPassword))
			{
				sys_log(0, "   OK");
				break;
			}

			sys_log(0, "   failed, retrying in 5 seconds");
			fprintf(stderr, "   failed, retrying in 5 seconds");
			sleep(5);
		} while (iRetry--);
		fprintf(stderr, "Success COMMON\n");
	}
	else
	{
		sys_err("SQL_COMMON not configured");
		return false;
	}

#ifndef DISABLE_HOTBACKUP_SYSTEM
	if (CConfig::instance().GetValue("SQL_HOTBACKUP", line, 256))
	{
		sscanf(line, " %s %s %s %s %d ", szAddr, szDB, szUser, szPassword, &iPort);
		sys_log(0, "connecting to MySQL server (hotbackup)");

		int iRetry = 5;

		do
		{
			if (CDBManager::instance().Connect(SQL_HOTBACKUP, szAddr, iPort, szDB, szUser, szPassword))
			{
				sys_log(0, "   OK");
				break;
			}

			sys_log(0, "   failed, retrying in 5 seconds");
			fprintf(stderr, "   failed, retrying in 5 seconds");
			sleep(5);
		}
		while (iRetry--);

		fprintf(stderr, "Success HOTBACKUP\n");
	}
	else
	{
		sys_err("SQL_HOTBACKUP not configured");
		return false;
	}
#endif
	
	if (!CNetPoller::instance().Create())
	{
		sys_err("Cannot create network poller");
		return false;
	}

	sys_log(0, "ClientManager initialization.. ");

	if (!CClientManager::instance().Initialize())
	{
		sys_log(0, "   failed"); 
		return false;
	}

	sys_log(0, "   OK");

#ifndef DISABLE_HOTBACKUP_SYSTEM
	if (!PlayerHB::instance().Initialize())
	{
		sys_err("cannot initialize player hotbackup");
		return false;
	}
#endif

#ifndef __WIN32__
	signal(SIGUSR1, emergency_sig);
#endif
	signal(SIGSEGV, emergency_sig);
	return true;
}

void SetTablePostfix(const char* c_pszTablePostfix)
{
	if (!c_pszTablePostfix || !*c_pszTablePostfix)
		g_stTablePostfix = "";
	else
		g_stTablePostfix = c_pszTablePostfix;
}

const char * GetTablePostfix()
{
	return g_stTablePostfix.c_str();
}

void SetPlayerDBName(const char* c_pszPlayerDBName)
{
	if (! c_pszPlayerDBName || ! *c_pszPlayerDBName)
		g_stPlayerDBName = "";
	else
	{
		g_stPlayerDBName = c_pszPlayerDBName;
		g_stPlayerDBName += ".";
	}
}

const char * GetPlayerDBName()
{
	return g_stPlayerDBName.c_str();
}


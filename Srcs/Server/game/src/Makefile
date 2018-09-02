PLATFORM = $(shell file /bin/ls | cut -d' ' -f3 | cut -d'-' -f1)

GCC_VERSION = $(shell $(CC) --version 2>&1 | grep "(GCC)" | cut -d' ' -f3  | cut -d'.' -f1)
BSD_VERSION = $(shell uname -v 2>&1 | cut -d' ' -f2 | cut -d'.' -f1)
SVN_VERSION = FRANCE


CC = g++6
CXX = g++6

INCDIR =
LIBDIR =
BINDIR = ..
OBJDIR = OBJDIR
$(shell if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi)

# Standard Setting
LIBS = -pthread -lm -lmd
# Removed -fno-rtti 
#CFLAGS = -Wall -fuse-linker-plugin -fuse-ld=gold -O2 -m32 -pipe -fexceptions -D_THREAD_SAFE -Wl,-rpath=/usr/local/lib32/metin2 -std=c++11 -static -static-libgcc -static-libstdc++ -Wno-strict-aliasing -fpermissive
CFLAGS = -fuse-linker-plugin -fuse-ld=gold -m32 -pipe -fexceptions -fpermissive -D_THREAD_SAFE -std=c++11 -Wno-strict-aliasing #-static -static-libgcc -static-libstdc++ -Wno-strict-aliasing # -D_THREAD_SAFE

#debug flags
CFLAGS += -ggdb3 -DNDEBUG
#end of debug flags

CFLAGS += -ffast-math -funsafe-loop-optimizations -ftracer -fira-loop-pressure -funroll-loops -finline-functions -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-loop-distribution -ftree-partial-pre -ftree-vectorize -ftree-loop-distribute-patterns -ftree-slp-vectorize -fipa-cp-clone -trigraphs
CFLAGS += -mtune=i686 -march=i686
CFLAGS += -mssse3
CFLAGS += -mssse3 -msse4
# boost
INCDIR += -I../../../Extern/include/boost

# DevIL
INCDIR += -I../../libdevil
LIBDIR += -L../../libdevil
LIBS += -lIL -lpng -ltiff -lmng -llcms -ljpeg

# MySQL
ifeq ($(BSD_VERSION), 7)
INCDIR += -I../../libmysql/7.x-5.1.35
LIBDIR += -L../../libmysql/7.x-5.1.35
else
INCDIR += -I../../libmysql/5.x-5.1.35
LIBDIR += -L../../libmysql/5.x-5.1.35
endif

LIBS += -lmysqlclient -lz

# Miscellaneous external libraries
INCDIR += -I../../../Extern/include
LIBDIR += -L../../../Extern/lib
LIBS += -lcryptopp -lgtest

# HackShield
INCDIR += -I../../libhackshield/include
LIBDIR += -L../../libhackshield/lib
LIBS += -lanticpxsvr

# XTrap
INCDIR += -I../../libxtrap/include

# openssl
#INCDIR += -I/usr/include
#LIBS += -lssl
#LIBS += /usr/lib/libssl.a

# Project Library
INCDIR += -I../../liblua/include
INCDIR += -I/usr/local/include
INCDIR += -I../../libserverkey
LIBDIR += -L../../libthecore/lib -L../../libpoly -L../../libsql -L../../libgame/lib -L../../liblua/lib -L../../libserverkey
LIBDIR += -L/usr/local/lib
LIBS += -lthecore -lpoly -llua -llualib -lsql -lgame -lserverkey
USE_STACKTRACE = 0
ifeq ($(USE_STACKTRACE), 1)
LIBS += /usr/local/lib/libexecinfo.a
endif

TARGET  = $(BINDIR)/game_r$(SVN_VERSION)_$(PLATFORM)

CFILE	= minilzo.c

CPPFILE = char_new_mouvement.cpp char_item_bloquer.cpp char_mob.cpp anti_cheat_pvp.cpp char_bot.cpp char_magasinier.cpp char_meteo.cpp char_environnement.cpp char_inventaire.cpp char_titre.cpp char_cadeau.cpp char_offlineshop.cpp acce.cpp BattleArena.cpp FSM.cpp MarkConvert.cpp MarkImage.cpp MarkManager.cpp OXEvent.cpp TrafficProfiler.cpp ani.cpp\
		  arena.cpp banword.cpp battle.cpp blend_item.cpp block_country.cpp buffer_manager.cpp building.cpp castle.cpp\
		  char.cpp char_affect.cpp char_battle.cpp char_change_empire.cpp char_horse.cpp char_item.cpp char_manager.cpp\
		  char_quickslot.cpp char_resist.cpp char_skill.cpp char_state.cpp PetSystem.cpp cmd.cpp cmd_emotion.cpp cmd_general.cpp\
		  cmd_gm.cpp cmd_oxevent.cpp config.cpp constants.cpp crc32.cpp cube.cpp db.cpp desc.cpp\
		  desc_client.cpp desc_manager.cpp desc_p2p.cpp dev_log.cpp dungeon.cpp empire_text_convert.cpp entity.cpp\
		  entity_view.cpp event.cpp event_queue.cpp exchange.cpp file_loader.cpp fishing.cpp gm.cpp guild.cpp\
		  guild_manager.cpp guild_war.cpp horse_rider.cpp horsename_manager.cpp input.cpp input_auth.cpp input_db.cpp\
		  input_login.cpp input_main.cpp input_p2p.cpp input_teen.cpp input_udp.cpp ip_ban.cpp\
		  item.cpp item_addon.cpp item_attribute.cpp item_manager.cpp item_manager_idrange.cpp locale.cpp\
		  locale_service.cpp log.cpp login_data.cpp lzo_manager.cpp marriage.cpp matrix_card.cpp\
		  messenger_manager.cpp mining.cpp mob_manager.cpp monarch.cpp motion.cpp over9refine.cpp p2p.cpp packet_info.cpp\
		  party.cpp passpod.cpp pcbang.cpp polymorph.cpp priv_manager.cpp pvp.cpp\
		  questevent.cpp unique.cpp questlua.cpp questlua_affect.cpp questlua_arena.cpp questlua_ba.cpp questlua_building.cpp\
		  questlua_danceevent.cpp questlua_dungeon.cpp questlua_forked.cpp questlua_mysql.cpp questlua_game.cpp questlua_global.cpp\
		  questlua_guild.cpp questlua_horse.cpp questlua_pet.cpp questlua_item.cpp questlua_marriage.cpp questlua_mgmt.cpp\
		  questlua_monarch.cpp questlua_npc.cpp questlua_oxevent.cpp questlua_party.cpp questlua_pc.cpp\
		  questlua_quest.cpp questlua_target.cpp questmanager.cpp questnpc.cpp questpc.cpp\
		  refine.cpp regen.cpp safebox.cpp sectree.cpp sectree_manager.cpp sequence.cpp shop.cpp\
		  skill.cpp start_position.cpp target.cpp text_file_loader.cpp trigger.cpp utils.cpp vector.cpp war_map.cpp\
		  wedding.cpp xmas_event.cpp version.cpp panama.cpp threeway_war.cpp map_location.cpp auth_brazil.cpp\
		  BlueDragon.cpp BlueDragon_Binder.cpp DragonLair.cpp questlua_dragonlair.cpp\
		  HackShield.cpp HackShield_Impl.cpp char_hackshield.cpp skill_power.cpp affect.cpp\
		  SpeedServer.cpp questlua_speedserver.cpp XTrapManager.cpp\
		  auction_manager.cpp FileMonitor_FreeBSD.cpp ClientPackageCryptInfo.cpp cipher.cpp\
		  buff_on_attributes.cpp dragon_soul_table.cpp DragonSoul.cpp\
		  group_text_parse_tree.cpp char_dragonsoul.cpp questlua_dragonsoul.cpp\
		  shop_manager.cpp shopEx.cpp item_manager_read_tables.cpp New_PetSystem.cpp questlua_petnew.cpp spamblock.cpp


COBJS   = $(CFILE:%.c=$(OBJDIR)/%.o)
CPPOBJS = $(CPPFILE:%.cpp=$(OBJDIR)/%.o)
 
MAINOBJ = $(OBJDIR)/main.o
MAINCPP = main.cpp
 
default: $(TARGET)
 
$(OBJDIR)/minilzo.o: minilzo.c
	@$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@
	@echo compile $<
 
$(OBJDIR)/version.o: version.cpp
	@$(CC) $(CFLAGS) -D__USER__=\"$(USER)\" -D__HOSTNAME__=\"$(HOSTNAME)\" -D__PWD__=\"$(PWD)\" -D__SVN_VERSION__=\"$(SVN_VERSION)\" -c $< -o $@
	@echo compile $<
 
$(OBJDIR)/%.o: %.cpp
	@echo compile $<
	@$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@
 
limit_time:
	@echo update limit time
	@python update_limit_time.py
 
$(TARGET): $(CPPOBJS) $(COBJS) $(MAINOBJ)
	@echo linking $(TARGET)....
	@$(CC) $(CFLAGS) $(LIBDIR) $(COBJS) $(CPPOBJS) $(MAINOBJ) $(LIBS) -o $(TARGET)
 
clean:
	@rm -f $(COBJS) $(CPPOBJS)
	@rm -f $(BINDIR)/game_r* $(BINDIR)/conv
 
tag:
	ctags *.cpp *.h *.c
 
dep:
	makedepend -f Depend $(INCDIR) -I/usr/include/c++/3.3 -I/usr/include/c++/4.2 -p$(OBJDIR)/ $(CPPFILE) $(CFILE) $(MAINCPP) $(TESTCPP) 2> /dev/null > Depend
 
sinclude Depend 

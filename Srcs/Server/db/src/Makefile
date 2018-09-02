MAKE = gmake
CC = g++6
CXX = g++6

INCDIR =
LIBDIR =
BINDIR = ..
OBJDIR = .obj

GCC_VERSION = $(shell $(CC) --version 2>&1 | grep "(GCC)" | cut -d' ' -f3  | cut -d'.' -f1)
BSD_VERSION = $(shell uname -v 2>&1 | cut -d' ' -f2 | cut -d'.' -f1)
P4_VERSION = FRANCE
$(shell if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi)

TARGET = $(BINDIR)/db_r$(P4_VERSION)

CFLAGS = -pthread -fuse-linker-plugin -fuse-ld=gold -m32 -pipe -fexceptions -fpermissive -D_THREAD_SAFE -std=c++14 -Wno-strict-aliasing #-static -static-libgcc -static-libstdc++ -Wno-strict-aliasing # -D_THREAD_SAFE

#debug flags
CFLAGS += -ggdb3 -DNDEBUG
#end of debug flags

CFLAGS += -ffast-math -funsafe-loop-optimizations -ftracer -fira-loop-pressure -funroll-loops -finline-functions -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-loop-distribution -ftree-partial-pre -ftree-vectorize -ftree-loop-distribute-patterns -ftree-slp-vectorize -fipa-cp-clone -trigraphs
CFLAGS += -mtune=k8 -march=x86-64
CFLAGS += -mssse3

# boost
INCDIR += -I../../boost

LIBDIR += -L../../libthecore/lib -L../../libsql -L../../libpoly -L../../libgame/lib

ifeq ($(BSD_VERSION), 7)
INCDIR += -I../../libmysql/7.x-5.1.35
LIBDIR += -L../../libmysql/7.x-5.1.35
else
INCDIR += -I../../libmysql/5.x-5.1.35
LIBDIR += -L../../libmysql/5.x-5.1.35
endif

LIBS = -lthecore -lmysqlclient -lsql -lpoly -lgame -lm -lz

SRCS =	Config.cpp NetBase.cpp Peer.cpp PeerBase.cpp Main.cpp Lock.cpp DBManager.cpp \
		Cache.cpp LoginData.cpp ClientManager.cpp ClientManagerPlayer.cpp ClientManagerLogin.cpp \
		ClientManagerBoot.cpp ClientManagerParty.cpp ClientManagerGuild.cpp GuildManager.cpp HB.cpp \
		PrivManager.cpp MoneyLog.cpp ItemAwardManager.cpp ClientManagerEventFlag.cpp Marriage.cpp \
		Monarch.cpp BlockCountry.cpp ItemIDRangeManager.cpp ClientManagerHorseName.cpp version.cpp \
		AuctionManager.cpp ProtoReader.cpp CsvReader.cpp 

OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)

default: $(TARGET) $(TEST_TARGET)


$(TARGET): $(OBJS)
	@echo linking ...
	@$(CC) $(CFLAGS) $(LIBDIR) $(OBJS) $(LIBS) -o $(TARGET)
	@touch version.cpp

$(TEST_TARGET): $(OBJS)
	@echo linking ...
	@$(CC) $(CFLAGS) $(LIBDIR) $(OBJS) $(LIBS) -o $(TEST_TARGET)
	@touch version.cpp

$(OBJDIR)/%.o: %.cpp
	@echo compile $<
	@$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@

$(OBJDIR)/version.o: version.cpp
	@$(CC) $(CFLAGS) -D__USER__=\"$(USER)\" -D__HOSTNAME__=\"$(HOSTNAME)\" -D__PWD__=\"$(PWD)\" -D__P4_VERSION__=\"$(P4_VERSION)\" -c $< -o $@
	@echo compile $<

$(OBJDIR):
	@mkdir $(OBJDIR)

clean:
	@rm -f $(OBJS) $(BINDIR)/db_r*

dep:
	@touch Depend
	makedepend -fDepend $(INCDIR) -I/usr/include/c++/3.3 -I/usr/include/c++/4.2 -p$(OBJDIR)/ $(SRCS) 2> /dev/null

sinclude Depend

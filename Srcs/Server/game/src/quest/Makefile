CPP = g++410
LIBDIREXT = /usr/local/lib32/metin2
#CPPFLAGS=-g -Wall -Og -m32 -pipe -fexceptions -D_THREAD_SAFE -DNDEBUG -Wl,-rpath=$(LIBDIREXT) -mtune=i686 -mssse3 -static
#CPPFLAGS=-g -Wall -Og -m32 -pipe -fexceptions -D_THREAD_SAFE -DNDEBUG -Wl,-rpath=$(LIBDIREXT) -mtune=i686 -mssse3
CPPFLAGS=-g0 -Wall -flto -fuse-linker-plugin -fuse-ld=gold -Ofast -m32 -pipe -fexceptions -D_THREAD_SAFE -Wl,--strip-all -Wl,-rpath,$(LIBDIREXT) -mtune=i686 -march=i686 -mssse3 -msse4 -std=c++11 -static -static-libgcc -static-libstdc++
INCLUDEDIR=../../../liblua/include
LIBDIR=../../../liblua/lib
LIBS=-llua -llualib -pthread -lm -lmd
all: qc

qc: qc.cc qc.o crc32.o
	$(CPP) $(CPPFLAGS) -o ./qc -I$(INCLUDEDIR) -L$(LIBDIR) -L/usr/local/lib32/metin2 qc.o crc32.o $(LIBS) 

crc32.o: crc32.cc
	$(CPP) $(CPPFLAGS) -c $< -I$(INCLUDEDIR) -L$(LIBDIR) -L/usr/local/lib32/metin2

qc.o: qc.cc
	$(CPP) $(CPPFLAGS) -c $< -I$(INCLUDEDIR) -L$(LIBDIR) -L/usr/local/lib32/metin2

clean:
	rm -f qc qc.o

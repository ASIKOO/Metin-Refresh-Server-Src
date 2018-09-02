LIBDIREXT = /usr/local/lib32/metin2
CPP = g++410
CPPFLAGS=-g -Wall -Og -m32 -pipe -fexceptions -D_THREAD_SAFE -DNDEBUG -Wl,-rpath=$(LIBDIREXT) -mtune=i686 -mssse3 -static
INCLUDEDIR=../../../liblua/include
LIBDIR=../../../liblua/lib
LIBS=-llua -pthread -lm -lmd -llualib
all: qc

qc: qc.cc qc.o crc32.o
	$(CPP) $(CPPFLAGS) -g -o ./qc -I$(INCLUDEDIR) -L$(LIBDIR) qc.o crc32.o $(LIBS)

crc32.o: crc32.cc
	$(CPP) $(CPPFLAGS) -c $< -I$(INCLUDEDIR) -L$(LIBDIR)

qc.o: qc.cc
	$(CPP) $(CPPFLAGS) -c $< -I$(INCLUDEDIR) -L$(LIBDIR)

clean:
	rm -f qc qc.o


CXX	= g++49
BIN = ./libpoly.a

GCC_VERSION = $(shell $(CC) --version 2>&1 | grep "(GCC)" | cut -d' ' -f3  | cut -d'.' -f1)

ifeq ($(GCC_VERSION), 4)
CFLAGS  = -Wall -Ofast -pipe -mtune=i686 -fno-exceptions -fno-rtti -lstdc++ -std=c++11 -march=i686 -msse4 -mssse3
else
CFLAGS  = -Wall -Ofast -pipe -mcpu=i686 -fno-exceptions -fno-rtti -lstdc++ -std=c++11 -march=i686 -msse4 -mssse3
endif

LIBS =

OBJFILES = Base.o Poly.o SymTable.o Symbol.o

default:
	$(MAKE) $(BIN)

$(BIN): $(OBJFILES)
	ar cru $(BIN) $(OBJFILES) $(LIBS)
	ranlib $(BIN)
	chmod 700 $(BIN)

clean:
	rm -f *.o
	rm -f $(BIN)

dep:
	touch Depend
	$(CXX) $(CFLAGS) -MM *.cc > Depend

$(OBJFILES):
	$(CXX) $(CFLAGS) -c $<

include Depend


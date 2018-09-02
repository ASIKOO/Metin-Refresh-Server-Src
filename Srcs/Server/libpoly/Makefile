CC	= g++6
CXX	= g++6
BIN = ./libpoly.a

GCC_VERSION = $(shell $(CC) --version 2>&1 | grep "(GCC)" | cut -d' ' -f3  | cut -d'.' -f1)

ifeq ($(GCC_VERSION), 4)
CFLAGS  = -O2 -pipe -mtune=i686 -fno-exceptions -fno-rtti
else
CFLAGS  = -O2 -pipe -mcpu=i686 -fno-exceptions -fno-rtti
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


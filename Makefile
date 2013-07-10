BINDIR=bin
INCDIR=include
SRCDIR=src

PLATFORM=ML3
FLAGS=-ansi -O -I$(INCDIR) -D$(PLATFORM)
CARDFLAGS=$(FLAGS) -Falu
SIMFLAGS=$(FLAGS) -DSIMULATOR -g

HEADERS=$(wildcard $(INCDIR)/*.h)
SOURCES=$(wildcard $(SRCDIR)/*.c)

SMARTCARD=$(BINDIR)/SBcred.smartcard-$(PLATFORM).alu
SIMULATOR=$(BINDIR)/SBcred.simulator-$(PLATFORM).hzx

all: simulator smartcard

fresh: clean all

$(BINDIR):
	mkdir -p $(BINDIR)

simulator: $(HEADERS) $(SOURCES) $(SIMULATOR)

$(SIMULATOR): $(HEADERS) $(SOURCES) $(BINDIR)
	hcl $(SIMFLAGS) $(SOURCES) -o $(SIMULATOR)

smartcard: $(HEADERS) $(SOURCES) $(SMARTCARD)

$(SMARTCARD): $(HEADERS) $(SOURCES) $(BINDIR)
	hcl $(CARDFLAGS) $(SOURCES) -o $(SMARTCARD)

clean:
	rm -rf $(BINDIR) $(SRCDIR)/*~ $(INCDIR)/*~

.PHONY: all clean fresh simulator smartcard

PERL = perl

INTERNAL_PMC_HEADERS = internal_pmc/context.h internal_pmc/file.h
HEADERS = $(INTERNAL_PMC_HEADERS) config.h lorito.h microcode.h interp.h loader.h pmc.h internal_pmc.h pmc_func.h

INTERNAL_PMC_COMPILED = internal_pmc/context.o internal_pmc/file.o
COMPILED = $(INTERNAL_PMC_COMPILED) main.o interp.o core.o loader.o pmc.o internal_pmc.o pmc_func.o

LASM_PMC_FILES = $(wildcard pmc/*.lasm)
LASM_PMC_COMPILED = $(patsubst %.lasm,%.ito,$(LASM_PMC_FILES))

TEST_FILES = $(wildcard t/*.t)
COMPILED_TESTS = $(patsubst %.t,%.ito,$(TEST_FILES))

INC_PATH=-I./

%.o: %.c $(HEADERS)
	@echo "=> $@"
	@$(CC) -g -c $(INC_PATH) $(CPPFLAGS) $(CFLAGS) -o $@ $<

lorito: $(COMPILED) $(HEADERS)
	@echo "=> $@"
	@$(CC) -g $(CFLAGS) $(CC_WARN) -o $@ $(COMPILED)

.PHONY: CLEAN
clean:
	rm lorito $(COMPILED)

%.ito: %.lasm
	$(PERL) lasm.pl < $< > $@

%.ito: %.t $(LASM_PMC_COMPILED)
	$(PERL) lasm.pl < $< > $@

.PHONY: help
help:
	@echo "Available Targets:"
	@echo "  lorito: The experimental lorito interp"
	@echo "  clean:  Remove the compile time objects"
	@echo "  test:   Run the test suite"

.PHONY: test
test: $(COMPILED_TESTS)
	prove --exec ./lorito t/*.ito

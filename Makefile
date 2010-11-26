PERL = perl
HEADERS = config.h lorito.h microcode.h interp.h loader.h pmc.h internal_pmc.h pmc_func.h
COMPILED = main.o interp.o core.o loader.o pmc.o internal_pmc.o pmc_func.o
TEST_FILES = $(wildcard t/*.t)
COMPILED_TESTS = $(patsubst %.t,%.ito,$(TEST_FILES))

%.o: %.c $(HEADERS)
	$(CC) -g -c $(CPPFLAGS) $(CFLAGS) -o $@ $<

lorito: $(COMPILED) $(HEADERS)
	$(CC) -g $(CFLAGS) $(CC_WARN) -o $@ $(COMPILED)

.PHONY: CLEAN
clean:
	rm lorito $(COMPILED)

%.ito: %.lasm
	$(PERL) lasm.pl < $< > $@

%.ito: %.t
	$(PERL) lasm.pl < $< > $@

.PHONY: help
help:
	@echo "Available Targets:"
	@echo "  lorito: The experimental lorito interp"
	@echo "  clean:  Remove the compile time objects"
	@echo "  test:   Run the test suite"

.PHONY: test
test: $(COMPILED_TESTS)
	prove --exec ./lorito t/00_sanity.ito
	prove --exec ./lorito t/90_fib.ito

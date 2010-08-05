HEADERS = config.h lorito.h microcode.h interp.h loader.h pmc.h internal_pmc.h
COMPILED = main.o interp.o core.o loader.o pmc.o internal_pmc.o

%.o: %.c $(HEADERS)
	$(CC) -g -c $(CPPFLAGS) $(CFLAGS) -o $@ $<

lorito: $(COMPILED) $(HEADERS)
	$(CC) -g $(CFLAGS) $(CC_WARN) -o $@ $(COMPILED)

clean:
	rm lorito $(COMPILED)

HEADERS = config.h lorito.h microcode.h
COMPILED = main.o core.o loader.o
lorito: $(COMPILED) $(HEADERS)
	cc $(CFLAGS) $(CC_WARN) -o $@ $(COMPILED)

HEADERS = config.h lorito.h microcode.h
COMPILED = main.o core.o loader.o

%.o: %.c $(HEADERS)
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ $<

lorito: $(COMPILED) $(HEADERS)
	$(CC) $(CFLAGS) $(CC_WARN) -o $@ $(COMPILED)

include config.mk

all: $(OBJS)
	$(CC) $(OPTS) $(OBJS) -o ncsnake

debug: OPTS += -g -O0
debug: clean all

%.o: %.c
	$(CC) -c $(OPTS) $< -o $@

clean:
	rm -f -- *.o
	rm -f ncsnake

install: all
	cp -- ncsnake /usr/local/bin/ncsnake

uninstall:
	[ -x /usr/local/bin/ncsnake ] && rm -f /usr/local/bin/ncsnake


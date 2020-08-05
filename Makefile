include config.mk

all: $(OBJS)
	$(CC) $(OPTS) $(OBJS) -o ncsnake

debug: OPTS += -g -Og
debug: clean all

%.o: %.c
	$(CC) -c $(OPTS) $< -o $@

clean:
	rm -f -- *.o
	rm -f ncsnake

install: OPTS += -O3
install: clean all
	cp -- ncsnake /usr/local/bin/ncsnake

uninstall:
	[ -x /usr/local/bin/ncsnake ] && rm -f /usr/local/bin/ncsnake


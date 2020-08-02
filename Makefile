include config.mk

all: $(OBJS)
	$(CC) $(OPTS) $(OBJS) -o ncsnake

clean: ncsnake
	rm -f ncsnake

install: all
	cp -- ncsnake /usr/local/bin/ncsnake

uninstall:
	[ -x /usr/local/bin/ncsnake ] && rm -f /usr/local/bin/ncsnake


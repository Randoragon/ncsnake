include config.mk

all: $(OBJS)
	$(CC) $(OPTS) $(OBJS) -o ncsnake

ncsnake.o: ncsnake.c
	$(CC) -c ncsnake.c

snake.o: snake.c
	$(CC) -c snake.c

stage.o: stage.c
	$(CC) -c stage.c

clean:
	rm -f -- *.o
	rm -f ncsnake

install: all
	cp -- ncsnake /usr/local/bin/ncsnake

uninstall:
	[ -x /usr/local/bin/ncsnake ] && rm -f /usr/local/bin/ncsnake


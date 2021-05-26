# See LICENSE file for copyright and license details.
# minecurses - curses(3) minesweeper game
.POSIX:

include config.mk

BIN = minecurses
DIST = ${BIN}-${VERSION}
MAN6 = ${BIN}.6

SRC = minecurses.c
OBJ = ${SRC:.c=.o}

all: options ${BIN}

options:
	@echo ${BIN} build options:
	@echo "CFLAGS	= ${CFLAGS}"
	@echo "LDFLAGS	= ${LDFLAGS}"
	@echo "CC	= ${CC}"

${BIN}: ${OBJ}
	${CC} ${LDFLAGS} ${OBJ} -o $@

.c.o:
	${CC} -c ${CFLAGS} $<

dist: clean
	mkdir -p ${DIST}
	cp -R config.mk defs.h Makefile minecurses.6 minecurses.c \
	       README.md ${DIST}
	tar -cf ${DIST}.tar ${DIST}
	gzip ${DIST}.tar
	rm -rf ${DIST}

run:
	./${BIN}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin ${DESTDIR}${MANPREFIX}/man6
	cp -f ${BIN} ${DESTDIR}${PREFIX}/bin
	cp -f ${MAN6} ${DESTDIR}${MANPREFIX}/man6
	sed "s/VERSION/${VERSION}/g" < ${MAN6} > ${DESTDIR}${MANPREFIX}/man6/${MAN6}
	chmod 755 ${DESTDIR}${PREFIX}/bin/${BIN}
	chmod 644 ${DESTDIR}${MANPREFIX}/man6/${MAN6}

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/${BIN} \
		${DESTDIR}${MANPREFIX}/man6/${MAN6}

clean:
	rm -f ${BIN} ${OBJ} ${DIST}.tar.gz *.core

.PHONY: all options clean dist install uninstall run

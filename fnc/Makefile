# See LICENSE file for copyright and license details.
# fnc - a simple finance program
.POSIX:

PREFIX = /usr/local
MAN_DIR = ${PREFIX}/share/man/man1
BIN_DIR = ${PREFIX}/bin

BIN = fnc
VERSION = 0
DIST = ${BIN}-${VERSION}
MAN1 = ${BIN}.1

SRC = fnc.go

all: ${BIN}

${BIN}: ${SRC}
	go build ${SRC}

dist: clean
	mkdir -p ${DIST}
	cp -fR fnc.1 fnc.go Makefile ${DIST}
	tar -cf ${DIST}.tar ${DIST}
	gzip ${DIST}.tar
	rm -rf ${DIST}

install: all
	mkdir -p ${DESTDIR}${BIN_DIR} ${DESTDIR}${MAN_DIR}
	cp -f ${BIN} ${BIN_DIR}
	cp -f ${MAN1} ${DESTDIR}${MAN_DIR}
	sed "s/VERSION/${VERSION}/g" < ${MAN1} > ${DESTDIR}${MAN_DIR}/${MAN1}
	chmod 755 ${DESTDIR}${BIN_DIR}/${BIN}
	chmod 644 ${DESTDIR}${MAN_DIR}/${MAN1}

uninstall:
	rm -f ${DESTDIR}${BIN_DIR}/${BIN}
	rm -f ${DESTDIR}${MAN_DIR}/${MAN1}

run:
	./${BIN}

clean:
	rm -f ${BIN} ${OBJ} ${DIST}.tar.gz

.PHONY: all options clean dist install uninstall run

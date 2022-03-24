# See LICENSE file for copyright and license details.
# shitcoin version
VERSION = 0.1

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man
# OpenBSD
#MANPREFIX = ${PREFIX}/man

# includes and libs
INCS = -Iinclude
LIBS = -Llib -lmd

# flags
CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=200809L \
	   -D_XOPEN_SOURCE=700 -DVERSION=\"${VERSION}\"
CFLAGS = -std=c99 -pedantic -Wall -Os ${INCS} ${CPPFLAGS}
LDFLAGS = ${LIBS}

# compiler
CC = cc

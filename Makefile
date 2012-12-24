CC	?= gcc
STRIP ?= strip
CFLAGS =  -Os
LDFLAGS = -lmpd -lasound
CFDEBUG = -pedantic -Wall -Wunused-parameter 

EXEC = gumon
SRCS = gumon.c
OBJS = ${SRCS:.c=.o}

PREFIX?=/usr/local
BINDIR=${PREFIX}/bin

all: ${EXEC}

.c.o:
	${CC} ${CFLAGS} -o $@ -c $<

${OBJS}: config.h

config.h:
	@echo creating $@ from config.def.h
	@cp config.def.h $@

${EXEC}: ${OBJS}
	${CC} ${LDFLAGS} -o ${EXEC} ${OBJS}
	${STRIP} -s ${EXEC}

debug: ${EXEC}
debug: CC += ${CFDEBUG}

clean:
	rm -rf ./*.o
	rm -rf ./${EXEC}

install: gumon
	test -d ${DESTDIR}${BINDIR} || mkdir -p ${DESTDIR}${BINDIR}
	install -m755 gumon ${DESTDIR}${BINDIR}/gumon
	install -m755 weather.sh ${DESTDIR}${BINDIR}/weather.sh

uninstall:
	rm -f ${DESTDIR}${BINDIR}/gumon
	rm -f ${DESTDIR}${BINDIR}/weather.sh

.PHONY: all debug clean install

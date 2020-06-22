PREFIX=
DESTDIR=/usr
CC=gcc
CFLAGS+= -shared
LIB_NAME=$(shell pwd | rev | cut -d'/' -f1 | rev )

MAN_SECTION="3"
MAN_SRC=$(shell find .man 2>/dev/null| grep -E "${LIB_NAME}.*\.${MAN_SECTION}\.gz" | rev | cut -d'/' -f1 | rev )

help:
	@echo "available cmd are make help|install|uninstall|clean"
	@echo "available args:"
	@echo "    LIB_NAME current \"${LIB_NAME}\""
	@echo "    CC current \"${CC}\""
	@echo "    DESTDIR current \"${DESTDIR}\""
	@echo "    PREFIX current \"${PREFIX}\""

lib%.so: %.c
	${CC} -o $@ $^ ${CFLAGS}

install: lib${LIB_NAME}.so
	install ${LIB_NAME}.h ${DESTDIR}/${PREFIX}/include
	install lib${LIB_NAME}.so ${DESTDIR}/${PREFIX}/lib
	@for path in ${MAN_SRC} ; do \
		echo add $$path to man ; \
		mkdir -p /usr/share/man/man${MAN_SECTION} ; \
		install .man/$$path /usr/share/man/man${MAN_SECTION} ; \
	done

uninstall:
	rm -f ${DESTDIR}/${PREFIX}/include/${LIB_NAME}.h
	rm -f ${DESTDIR}/${PREFIX}/lib/lib${LIB_NAME}.so
	@for path in ${MAN_SRC} ; do \
		echo rm $$path form man ; \
		rm -f /usr/share/man/man${MAN_SECTION}/$$path ; \
	done

clean:
	rm lib${LIB_NAME}.so

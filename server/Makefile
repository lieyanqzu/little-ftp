include ../Make.defines

PROGS = server

all:	${PROGS}

server:	main.o listen.o client_wait.o ctl_handle.o ../common/common.o
	${CC} ${CFLAGS} -o $@ $^ ${LIBS}

clean:
	rm -f ${PROGS} ${CLEANFILES}
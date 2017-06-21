include ../Make.defines

PROGS = client

all:	${PROGS}

client:	main.o connect.o ctl_handle.o ../common/common.o
	${CC} ${CFLAGS} -o $@ $^ ${LIBS}

clean:
	rm -f ${PROGS} ${CLEANFILES} 
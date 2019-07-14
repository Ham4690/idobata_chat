#
# Makefile for idobata_chat
#
MYLIBDIR=../mynet
 MYLIB=-lmynet
 CFLAGS=-I${MYLIBDIR} -L${MYLIBDIR}
# CFLAGS=-I${MYLIBDIR} 
#OBJS=idobata_chat.o idobata_server.o idobata_client.o idobata_util.o
OBJS=idobata_chat.o 

all:idobata

idobata : ${OBJS}
	 ${CC} ${CFLAGS} -o $@ $^ ${MYLIB}
	# ${CC} ${CFLAGS} -o $@ $^ 

clean:
	${RM} *.o idobata *~
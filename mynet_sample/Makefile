#CC := gcc
MYLIBD = -lmynet
CFLAGS := -I../mynet -L../mynet

all: echo_client echo_server

echo_client:echo_client2.c
	${CC} ${CFLAGS} -o $@ $^ ${MYLIBD}

echo_server:echo_server2.c
	${CC} ${CFLAGS} -o $@ $^ ${MYLIBD}

clean:
	${RM} echo_client echo_server

#CC := gcc
MYLIBD = -lmynet
CFLAGS := -I../mynet -L../mynet

all: echo_client echo_server

echo_client:echo_client.c
	${CC} ${CFLAGS} -o $@ $^ ${MYLIBD}

echo_server:echo_server.c
	${CC} ${CFLAGS} -o $@ $^ ${MYLIBD}

clean:
	${RM} echo_client echo_server

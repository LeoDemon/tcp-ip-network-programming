hello_client: hello_client.o log.o
	gcc -o hello_client hello_client.o log.o

hello_server: hello_server.o log.o
	gcc -o hello_server hello_server.o log.o

log.o: log.c
	gcc -c log.c
hello_client.o: hello_client.c
	gcc -c hello_client.c
hello_server.o: hello_server.c
	gcc -c hello_server.c

clean:
	rm -f *.o hello_client hello_server

.PHONY: clean

CFLAGS = -O2 -Wall -I. -Icommon

COMMON_OBJS=common/log.o

CH01_CLIENT_OBJS=ch01/hello_client.o
CH01_SERVER_OBJS=ch01/hello_server.o

CH02_CLIENT_OBJS=ch02/hello_client.o
CH02_SERVER_OBJS=ch02/hello_server.o

ch01: $(COMMON_OBJS) $(CH01_CLIENT_OBJS) $(CH01_SERVER_OBJS)
	gcc $(CFLAGS) -o ch01/hello_client $(COMMON_OBJS) $(CH01_CLIENT_OBJS)
	gcc $(CFLAGS) -o ch01/hello_server $(COMMON_OBJS) $(CH01_SERVER_OBJS)

ch02: $(COMMON_OBJS) $(CH02_CLIENT_OBJS) $(CH02_SERVER_OBJS)
	gcc $(CFLAGS) -o ch02/hello_client $(COMMON_OBJS) $(CH02_CLIENT_OBJS)
	gcc $(CFLAGS) -o ch02/hello_server $(COMMON_OBJS) $(CH02_SERVER_OBJS)

common/%.o: common/%.c
	gcc $(CFLAGS) -c $< -o $@

ch01/%.o: ch01/%.c
	gcc $(CFLAGS) -c $< -o $@

ch02/%.o: ch02/%.c
	gcc $(CFLAGS) -c $< -o $@

clean01:
	rm -f common/*.o ch01/*.o ch01/hello_client ch01/hello_server

clean02:
	rm -f common/*.o ch02/*.o ch02/hello_client ch02/hello_server

.PHONY: clean

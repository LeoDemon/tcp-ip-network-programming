# macOS has no epoll lib and header files, so we should compile this on Linux Platform.
add_executable(echo_epollserv echo_epollserv.c)
add_executable(echo_epoll_lt_serv echo_epoll_lt_serv.c)
add_executable(echo_epoll_et_serv echo_epoll_et_serv.c)

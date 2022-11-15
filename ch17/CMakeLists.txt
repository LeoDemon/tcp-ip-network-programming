# macOS has no epoll lib and header files, so we should compile this on Linux Platform.
add_executable(echo_epollserv echo_epollserv.c)

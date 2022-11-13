add_executable(stdcpy stdcpy.c)
target_link_libraries(stdcpy tcp_ip_common)

add_executable(syscpy syscpy.c)
target_link_libraries(syscpy tcp_ip_common)

add_executable(fileno_fdopen fileno_fdopen.c)

add_executable(echo_stdserver echo_stdserver.c)
target_link_libraries(echo_stdserver tcp_ip_common)

add_executable(echo_stdclient echo_stdclient.c)
target_link_libraries(echo_stdclient tcp_ip_common)

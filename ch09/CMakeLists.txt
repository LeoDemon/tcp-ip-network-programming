add_executable(sock_type sock_type.c)
target_link_libraries(sock_type tcp_ip_common)

add_executable(sock_buf sock_buf.c)
target_link_libraries(sock_buf tcp_ip_common)

add_executable(oob_send oob_send.c)
target_link_libraries(oob_send tcp_ip_common)

add_executable(oob_recv oob_recv.c)
target_link_libraries(oob_recv tcp_ip_common)

add_executable(peek_send peek_send.c)
target_link_libraries(peek_send tcp_ip_common)

add_executable(peek_recv peek_recv.c)
target_link_libraries(peek_recv tcp_ip_common)

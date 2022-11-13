add_executable(sep_client sep_client.c)
target_link_libraries(sep_client tcp_ip_common)

add_executable(sep_server sep_server.c)
target_link_libraries(sep_server tcp_ip_common)

add_executable(dup dup.c)

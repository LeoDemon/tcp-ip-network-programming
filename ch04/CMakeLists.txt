add_executable(echo_client echo_client.c)
target_link_libraries(echo_client tcp_ip_common)

add_executable(echo_server echo_server.c)
target_link_libraries(echo_server tcp_ip_common)

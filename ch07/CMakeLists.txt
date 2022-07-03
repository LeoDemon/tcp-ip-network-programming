add_executable(file_client file_client.c)
target_link_libraries(file_client tcp_ip_common)

add_executable(file_server file_server.c)
target_link_libraries(file_server tcp_ip_common)

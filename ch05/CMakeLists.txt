add_executable(echo_client2 echo_client2.c)
target_link_libraries(echo_client2 tcp_ip_common)

add_executable(echo_server2 echo_server2.c)
target_link_libraries(echo_server2 tcp_ip_common)

add_executable(op_client op_client.c)
target_link_libraries(op_client tcp_ip_common)

add_executable(op_server op_server.c)
target_link_libraries(op_server tcp_ip_common)

add_executable(op_client2 op_client2.c)
target_link_libraries(op_client2 tcp_ip_common)

add_executable(op_server2 op_server2.c)
target_link_libraries(op_server2 tcp_ip_common)

add_executable(char_int_array char_int_array.c)
target_link_libraries(char_int_array tcp_ip_common)

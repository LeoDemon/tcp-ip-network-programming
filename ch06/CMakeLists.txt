add_executable(uecho_client uecho_client.c)
target_link_libraries(uecho_client tcp_ip_common)

add_executable(uecho_server uecho_server.c)
target_link_libraries(uecho_server tcp_ip_common)

add_executable(bound_host1 bound_host1.c)
target_link_libraries(bound_host1 tcp_ip_common)

add_executable(bound_host2 bound_host2.c)
target_link_libraries(bound_host2 tcp_ip_common)

add_executable(uecho_con_client uecho_con_client.c)
target_link_libraries(uecho_con_client tcp_ip_common)

add_executable(test_sockaddr test_sockaddr.c)

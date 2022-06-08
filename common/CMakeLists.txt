set(tcp_ip_common_lib log.c datetime.c ../ch06/uecho_server.c ../ch06/uecho_client.c socket_util.c socket_util.h)

add_library(tcp_ip_common ${tcp_ip_common_lib})
target_link_libraries(tcp_ip_common pthread)

install(TARGETS tcp_ip_common DESTINATION lib)

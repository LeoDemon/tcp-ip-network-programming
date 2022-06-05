set(tcp_ip_common_lib log.c datetime.c)

add_library(tcp_ip_common ${tcp_ip_common_lib})
target_link_libraries(tcp_ip_common pthread)

install(TARGETS tcp_ip_common DESTINATION lib)

add_executable(domain_to_ip domain_to_ip.c hostent.c hostent.h)
target_link_libraries(domain_to_ip tcp_ip_common)

add_executable(ip_to_domain ip_to_domain.c hostent.c hostent.h)
target_link_libraries(ip_to_domain tcp_ip_common)

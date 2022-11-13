add_executable(stdcpy stdcpy.c)
target_link_libraries(stdcpy tcp_ip_common)

add_executable(syscpy syscpy.c)
target_link_libraries(syscpy tcp_ip_common)

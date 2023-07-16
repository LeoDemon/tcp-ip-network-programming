add_compile_definitions(_REENTRANT)

add_executable(web_server web_server.c)
target_link_libraries(web_server pthread)

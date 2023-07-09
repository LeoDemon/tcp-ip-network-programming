# 为所有目标添加编译选项
add_compile_definitions(_REENTRANT)

add_executable(thread_1801 thread_1801.c)
target_link_libraries(thread_1801 pthread)

add_executable(thread_1802 thread_1802.c)
target_link_libraries(thread_1802 pthread)

add_executable(thread_1803 thread_1803.c)
target_link_libraries(thread_1803 pthread)

add_executable(thread_mutex thread_mutex.c)
# 为某个指定目标添加编译选项
target_compile_definitions(thread_mutex PUBLIC _REENTRANT)
target_link_libraries(thread_mutex pthread)

add_executable(thread_semaphore thread_semaphore.c)
target_link_libraries(thread_semaphore pthread)

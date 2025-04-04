cmake_minimum_required(VERSION 3.30)
project(IPC_Threads_DB)

set(CMAKE_CXX_STANDARD 20)

# Cliente
add_executable(cliente cliente.c)

# Servidor
add_executable(servidor servidor.c banco.h)

# Se usar pthread em algum dos dois
target_link_libraries(cliente pthread)
target_link_libraries(servidor pthread)

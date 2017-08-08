cmake_minimum_required(VERSION 3.8)
project(Making_Threads)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        nbproject/private/c_standard_headers_indexer.c
        nbproject/private/cpp_standard_headers_indexer.cpp
        Part1/littleThread.h
        Part1/OSA1.1.backup.c
        Part1/OSA1.1.c
        Part1/threads1.c
        Part2/littleThread.h
        Part2/OSA1.2.c
        Part2/threads2.c
        Part3/littleThread.h
        Part3/threads3.c
        littleThread.h
        OSA1.c
        threads0.c)

add_executable(Making_Threads ${SOURCE_FILES})
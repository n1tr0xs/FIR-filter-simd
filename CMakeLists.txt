cmake_minimum_required(VERSION 3.10)
project(FIRFilterSIMD)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включить максимальную оптимизацию
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Включаем флаги оптимизации и SIMD-инструкции (SSE/AVX в зависимости от CPU)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -ffast-math")

add_executable(fir_simd main.cpp)

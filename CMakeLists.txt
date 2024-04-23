cmake_minimum_required(VERSION 3.5)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

add_compile_options(-Werror -g2 -O3 -fPIC -Wextra -Wall -Wno-strict-aliasing -Wno-stringop-truncation) # -Wpedantic)

project(STC_SFM_TEST)

set(CMAKE_CXX_FLAGS "-std=c++17")

set(SOURCE_EXE main.cpp)
set(SFM_LIB SFM.cpp)
set(TEST_LIB MyTests.cpp)
set(DATA_GENERATOR_LIB DataGenerator.cpp)

add_library(sfm STATIC ${SFM_LIB})
add_library(myTest STATIC ${TEST_LIB})
add_library(dataGenerator STATIC ${DATA_GENERATOR_LIB})

include_directories(SimpleXlsx)

add_executable(main ${SOURCE_EXE})

add_subdirectory(SimpleXlsx)

target_link_libraries(main sfm myTest dataGenerator SimpleXlsx)
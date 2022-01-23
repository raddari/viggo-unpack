cmake_minimum_required(VERSION 3.20)
project(viggo C)

set(UNPACKER_EXE ${PROJECT_NAME}-unpacker)

add_executable(${UNPACKER_EXE})
target_include_directories(${UNPACKER_EXE} PRIVATE
  ${CMAKE_SOURCE_DIR}/include/viggo)

add_subdirectory(src)

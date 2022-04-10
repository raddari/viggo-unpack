cmake_minimum_required(VERSION 3.20)
project(viggo C)

set(UNPACKER_EXE ${PROJECT_NAME}-unpacker)

include(FetchContent)
FetchContent_Declare(
  radian
  GIT_REPOSITORY https://github.com/raddari/radian.git
  GIT_TAG main)
FetchContent_MakeAvailable(radian)

add_executable(${UNPACKER_EXE})
target_include_directories(${UNPACKER_EXE} PRIVATE
  ${CMAKE_SOURCE_DIR}/include/viggo)
target_link_libraries(${UNPACKER_EXE} PRIVATE
  radian::radian)

add_subdirectory(src)

cmake_minimum_required(VERSION 3.12)
project(sessions)

add_library(sessions)

target_include_directories(sessions PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(sessions PUBLIC cxx_std_17)
target_sources(sessions PRIVATE
  $<$<NOT:$<PLATFORM_ID:Windows>>:src/posix.cxx>
  $<$<PLATFORM_ID:Windows>:src/windows.cxx>)

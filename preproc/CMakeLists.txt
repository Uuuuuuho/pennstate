cmake_minimum_required(VERSION 3.10)
project(InactiveCodeTracker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find LLVM
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Add the executable
add_executable(InactiveCodeTracker InactiveCodeTracker.cpp)

# Link libraries
target_link_libraries(InactiveCodeTracker
  PRIVATE
    clangTooling
    clangFrontend
    clangDriver
    clangParse
    clangSema
    clangAnalysis
    clangEdit
    clangLex
    clangBasic
    clangRewrite
    clangAST
    ${LLVM_LIBS}
)

# Add link directories
target_link_directories(InactiveCodeTracker PRIVATE ${LLVM_LIBRARY_DIRS})

# Set output directory
set_target_properties(InactiveCodeTracker PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

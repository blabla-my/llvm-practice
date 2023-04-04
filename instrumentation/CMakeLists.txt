cmake_minimum_required(VERSION 3.4.3)

cmake_policy(SET CMP0074 NEW)
cmake_policy(SET CMP0077 NEW)
set(LLVM_ROOT "/home/vm4/llvm-project/build" CACHE PATH "Root of LLVM install.")
list(APPEND CMAKE_PREFIX_PATH "${LLVM_ROOT}/lib/cmake/llvm")

find_package(LLVM REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(HandleLLVMOptions) # load additional config
include(AddLLVM) # used to add our own modules
set(CMAKE_CXX_STANDARD 17 CACHE STRING "")

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})

add_subdirectory(InstrumentFunctions)

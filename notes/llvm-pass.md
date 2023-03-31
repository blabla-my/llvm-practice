# LLVM PASS
LLVM PASS can be used to perform instrumentation and static analysis. 

## Build Pass
Pass can be directly build under the llvm source project, and also can be build out-of-tree.

Follow is the example of building an out-of-tree pass.

The key point is to include some cmake settings of llvm. Specifically: 
- Add path of `$llvm_installation/lib/cmake/llvm` to the $CMAKE_PATH_PREFIX.
- use `find_package(LLVM REQUIRED CONFIG)` to fetch llvm cmake package.

Then, we can use llvm cmake macros.
```cmake
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(HandleLLVMOptions) # load additional config
include(AddLLVM) # used to add our own modules
set(CMAKE_CXX_STANDARD 17 CACHE STRING "")

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
```
Finally, add your pass project:
```cmake
add_subdirectory(InstrumentFunctions)
```

The whole cmake file:
```cmake
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
```

## Trace function calls by instrumenting log functions
Very good proof of concept at : https://github.com/imdea-software/LLVM_Instrumentation_Pass

Basically, it runs a module pass, inserting some log functions to the entry point of every function in the module.
The inserted function is defined outside the target object file which needs to be instrumented.
LLVM library can compile the functions to bitcode, and insert them into the passed module's bitcode.

Some observations:
- Inserting functions declarations: definitions can be anywhere, remember to link the definition. 
- Inserting function calls: 



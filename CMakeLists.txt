find_package(LLVM REQUIRED CONFIG)

add_definitions(${LLVM_DEFINITIONS})
add_definitions(-std=c++17)
include_directories(${LLVM_INCLUDE_DIRS})
SET(CMAKE_CXX_FLAGS "-Wall -fno-rtti")

# add_subdirectory(lhyscg)
# add_subdirectory(newFuncNameCallGraph)
add_subdirectory(pass1)
add_subdirectory(pass2)
add_subdirectory(pass3)
add_subdirectory(pass4)

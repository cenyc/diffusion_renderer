cmake_minimum_required(VERSION 3.0.0)
project(diffusion_renderer VERSION 0.1.0)

include(CTest)
enable_testing()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include_directories(/home/cenyc/Library/enoki/include)

link_directories(/home/cenyc/Library/enoki/build)
# link_directories(/home/cenyc/Library/enoki/build/enoki)

# link_libraries加在add_executable之前
# link_libraries(enoki-cuda enoki-autodiff)

set(SOURCE_FILES Utils.cpp Camera.cpp Scene.cpp)
add_library(self_define ${SOURCE_FILES})

add_executable(diffusion_renderer main.cpp)

# target_link_libraries加在add_executable之后
target_link_libraries(diffusion_renderer -lenoki-cuda -lenoki-autodiff -lcuda)
target_link_libraries(diffusion_renderer self_define)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

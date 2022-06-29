cmake_minimum_required(VERSION 3.20)

project(adaptiveFilter CXX)

find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

find_path(CPP_HTTPLIB_INCLUDE_DIRS "httplib.h")
find_package(jsoncpp CONFIG REQUIRED)

add_library(adaptiveFilterLib STATIC
    AdaptiveFIR.h
    AdaptiveFIR.cpp
    Matrix.h
    Matrix.cpp
    WhiteNoise.h
    WhiteNoise.cpp
    Fir.h
    Fir.cpp
    JsonServer.h
    JsonServer.cpp
)
target_include_directories(adaptiveFilterLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(adaptiveFilterLib PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(adaptiveFilterLib PUBLIC jsoncpp_lib jsoncpp_object JsonCpp::JsonCpp)
target_include_directories(adaptiveFilterLib PUBLIC ${CPP_HTTPLIB_INCLUDE_DIRS})

add_executable(adaptiveFilter
    ExampleFir.h
    main.cpp
)
target_link_libraries(adaptiveFilter PUBLIC adaptiveFilterLib)

add_subdirectory(test)

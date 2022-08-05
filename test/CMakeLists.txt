set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SRC
    main.cpp
    AdaptiveFirTest.cpp
    MatrixTest.cpp
    WhiteNoiseTest.cpp
    IIRTest.cpp
)

find_package(GTest CONFIG REQUIRED)

add_executable(test ${SRC})
target_link_libraries(test
    PRIVATE
        GTest::gmock
        GTest::gtest
        GTest::gmock_main
        GTest::gtest_main
)
target_link_libraries(test
    PRIVATE
        adaptiveFilterLib
        iir
        noise
        fft
)
target_link_libraries(test PRIVATE jsoncpp_lib jsoncpp_object JsonCpp::JsonCpp)
target_include_directories(test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(test PRIVATE ${CPP_HTTPLIB_INCLUDE_DIRS})

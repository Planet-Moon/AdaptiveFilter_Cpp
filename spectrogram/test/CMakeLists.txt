set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(GTest CONFIG REQUIRED)

add_executable(spectrogram_test
main.cpp
Spectrogram_test.cpp
InputThread_test.cpp
)

target_link_libraries(spectrogram_test
    PRIVATE
        GTest::gmock
        GTest::gtest
        GTest::gmock_main
        GTest::gtest_main
)

target_link_libraries(spectrogram_test
    PRIVATE
        spectrogram
        signal_generators
)
target_include_directories(spectrogram_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

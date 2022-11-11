target_include_directories(test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
find_package(GTest CONFIG REQUIRED)

set(SRC
    main.cpp
    kalman_test.cpp
    kalman_multidim_test.cpp
    alpha_beta_gamma_test.cpp
)

add_executable(kalman_test ${SRC})
target_link_libraries(kalman_test
    PRIVATE
        GTest::gmock
        GTest::gtest
        GTest::gmock_main
        GTest::gtest_main
)
target_link_libraries(kalman_test
    PRIVATE
        kalman
        matrix
)

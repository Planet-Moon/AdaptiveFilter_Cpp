add_library(kalman kalman.cpp kalman.h)
target_include_directories(kalman PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(kalman PUBLIC matrix)

add_subdirectory(test)

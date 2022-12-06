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

add_library(matrix STATIC
    Matrix/Matrix.h
    Matrix/Matrix.cpp
)
target_include_directories(matrix PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Matrix)
target_link_libraries(matrix PUBLIC OpenMP::OpenMP_CXX)

add_library(filterreturntypes STATIC
    FilterReturnTypes/FilterReturnTypes.h
    FilterReturnTypes/FilterReturnTypes.cpp
)
target_include_directories(filterreturntypes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/FilterReturnTypes)
target_link_libraries(filterreturntypes PUBLIC matrix)
target_link_libraries(filterreturntypes PUBLIC jsoncpp_lib jsoncpp_object JsonCpp::JsonCpp)

add_library(fir STATIC
    Fir/Fir.h
    Fir/Fir.cpp
)
target_include_directories(fir PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Fir)
target_link_libraries(fir PUBLIC matrix filterreturntypes)

add_library(iir STATIC
    Iir/Iir.h
    Iir/Iir.cpp
)
target_include_directories(iir PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Iir)
target_link_libraries(iir PUBLIC matrix filterreturntypes)

add_library(FftRealPair STATIC
    FFT/RealPair/FftRealPair.cpp
    FFT/RealPair/FftRealPair.hpp
)
target_include_directories(FftRealPair PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/FFT/RealPair)
add_executable(FftRealPairTest FFT/RealPair/FftRealPairTest.cpp)
target_link_libraries(FftRealPairTest FftRealPair)

add_library(FftComplex STATIC
    FFT/Complex/FftComplex.cpp
    FFT/Complex/FftComplex.hpp
)
target_include_directories(FftComplex PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/FFT/Complex)
add_executable(FftComplexTest FFT/Complex/FftComplexTest.cpp)
target_link_libraries(FftComplexTest FftComplex)

add_library(fft STATIC
    FFT/FFT.h
    FFT/FFT.cpp
)
target_include_directories(fft PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/FFT)
target_link_libraries(fft PUBLIC FftComplex)
target_link_libraries(fft PUBLIC OpenMP::OpenMP_CXX)

add_executable(fft_stream
    FFT/Stream/DataSource.h
    FFT/Stream/DataSource.cpp
    FFT/Stream/CLI_DataSource.h
    FFT/Stream/CLI_DataSource.cpp
    FFT/Stream/HTTP_DataSource.h
    FFT/Stream/HTTP_DataSource.cpp
    FFT/Stream/DataPoint.h
    FFT/Stream/DataPoint.cpp
    FFT/Stream/FFTStream.h
    FFT/Stream/FFTStream.cpp
    FFT/Stream/FFTStreamRun.cpp
)
target_include_directories(fft_stream PRIVATE ${CPP_HTTPLIB_INCLUDE_DIRS})
target_link_libraries(fft_stream PRIVATE fft)
target_link_libraries(fft_stream PRIVATE jsoncpp_lib jsoncpp_object JsonCpp::JsonCpp)

add_executable(fft_stream_test
    FFT/Stream/test/SinusGenerator.h
    FFT/Stream/test/SinusGenerator.cpp
    FFT/Stream/test/FFTStreamTest.cpp
)
target_include_directories(fft_stream_test PRIVATE ${CPP_HTTPLIB_INCLUDE_DIRS})
target_link_libraries(fft_stream_test PRIVATE jsoncpp_lib jsoncpp_object JsonCpp::JsonCpp)
target_link_libraries(fft_stream_test PRIVATE noise)

add_executable(fft_stream_combined
    FFT/Stream/DataPoint.h
    FFT/Stream/DataPoint.cpp
    FFT/Stream/test/SinusGenerator.h
    FFT/Stream/test/SinusGenerator.cpp
    FFT/Stream/test/RectGenerator.h
    FFT/Stream/test/RectGenerator.cpp
    FFT/Stream/test/FFTStreamCombined.cpp
)
target_include_directories(fft_stream_combined PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/FFT/Stream)
target_include_directories(fft_stream_combined PRIVATE ${CPP_HTTPLIB_INCLUDE_DIRS})
target_link_libraries(fft_stream_combined PRIVATE jsoncpp_lib jsoncpp_object JsonCpp::JsonCpp)
target_link_libraries(fft_stream_combined PRIVATE fft noise)

add_library(noise STATIC
    Noise/WhiteNoise.h
    Noise/WhiteNoise.cpp
)
target_include_directories(noise PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Noise)

add_library(jsonserver STATIC
    Server/JsonServer.h
    Server/JsonServer.cpp
)
target_include_directories(jsonserver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Server)
target_include_directories(jsonserver PUBLIC ${CPP_HTTPLIB_INCLUDE_DIRS})
target_link_libraries(jsonserver PUBLIC jsoncpp_lib jsoncpp_object JsonCpp::JsonCpp)

add_library(adaptiveRLS STATIC
    adaptive_rls/AdaptiveRLS.h
    adaptive_rls/AdaptiveRLS.cpp
)
target_include_directories(adaptiveRLS PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adaptive_rls)
target_link_libraries(adaptiveRLS PUBLIC fir)

add_library(adaptiveLMS STATIC
    adaptive_lms/AdaptiveLMS.h
    adaptive_lms/AdaptiveLMS.cpp
)
target_include_directories(adaptiveLMS PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adaptive_lms)
target_link_libraries(adaptiveLMS PUBLIC fir)

add_library(adaptiveFilterLib INTERFACE)
target_include_directories(adaptiveFilterLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(adaptiveFilterLib INTERFACE
    adaptiveRLS
    adaptiveLMS
)

add_executable(SysIdentRLS
    ExampleFir.h
    SysIdent_RLS.cpp
)
target_link_libraries(SysIdentRLS PUBLIC
    adaptiveRLS
    noise
    jsonserver
)


add_executable(SysIdentLMS
    ExampleFir.h
    SysIdent_LMS.cpp
)
target_link_libraries(SysIdentLMS PUBLIC
    adaptiveLMS
    noise
    jsonserver
)

add_executable(Compare_LMS_RLS Compare_LMS_RLS.cpp)
target_link_libraries(Compare_LMS_RLS PUBLIC adaptiveLMS adaptiveRLS noise jsonserver)

add_subdirectory(test)

add_executable(PlotTest EXCLUDE_FROM_ALL PlotTest.cpp)
find_package(sciplot CONFIG REQUIRED)
target_link_libraries(PlotTest PRIVATE sciplot::sciplot)

add_subdirectory(kalman)

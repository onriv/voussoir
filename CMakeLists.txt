cmake_minimum_required(VERSION 2.8)

PROJECT(voussoir CXX)

set(ROOT ./)

include_directories(${ROOT})

ADD_EXECUTABLE(voussoir main.cpp marker.cpp page.cpp)

set(OpenCV_DIR /usr/local/Cellar/opencv@3/3.4.14_3/share/OpenCV/)
FIND_PACKAGE(OpenCV REQUIRED)

TARGET_LINK_LIBRARIES(voussoir ${OpenCV_LIBS})

##############
# For getting docopt to work
##############

target_compile_definitions(voussoir PRIVATE DOCOPT_HEADER_ONLY=1) # This is added because of an if statement at the bottom of docopt.h -- this enables actually including docopt.cpp from docopt.h.
set(CMAKE_CXX_FLAGS "-std=c++11") # docopt needs this, per https://github.com/Qihoo360/logkafka/issues/1

##############

FILE(COPY ${PROJECT_SOURCE_DIR}/Example_Images/test_input.jpg    DESTINATION ${PROJECT_BINARY_DIR}/bin/docs)
FILE(COPY ${PROJECT_SOURCE_DIR}/markers/markers_for_book_scanner.ai    DESTINATION ${PROJECT_BINARY_DIR}/bin/docs)
FILE(COPY ${PROJECT_SOURCE_DIR}/markers/markers_for_book_scanner.pdf    DESTINATION ${PROJECT_BINARY_DIR}/bin/docs)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

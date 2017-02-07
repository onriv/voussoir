cmake_minimum_required(VERSION 2.8)

PROJECT(bookscan)

set(ROOT ./)

include_directories(${ROOT})

ADD_EXECUTABLE(bookscan main.cpp marker.cpp page.cpp)

FIND_PACKAGE(OpenCV REQUIRED)

TARGET_LINK_LIBRARIES(bookscan ${OpenCV_LIBS})

##############
# For getting docopt to work
##############

target_compile_definitions(bookscan PRIVATE DOCOPT_HEADER_ONLY=1) # This is added because of an if statement at the bottom of docopt.h -- this enables actually including docopt.cpp from docopt.h.
set(CMAKE_CXX_FLAGS "-std=c++11") # docopt needs this, per https://github.com/Qihoo360/logkafka/issues/1

##############

FILE(COPY ${PROJECT_SOURCE_DIR}/Example_Images/test_input.jpg    DESTINATION ${PROJECT_BINARY_DIR}/bin)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

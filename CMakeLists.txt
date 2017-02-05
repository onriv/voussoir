cmake_minimum_required(VERSION 2.8)
 
set(ROOT ./)
 
include_directories(${ROOT} ./docopt-0.6.2)
#link_directories(${ROOT}/docopt-0.6.2)
 
ADD_EXECUTABLE(main main.cpp marker.cpp page.cpp)
 
FIND_PACKAGE(OpenCV REQUIRED)
 
TARGET_LINK_LIBRARIES(main ${OpenCV_LIBS})
 
set(CMAKE_CXX_FLAGS "-std=c++11") # docopt needs this, per https://github.com/Qihoo360/logkafka/issues/1
 
FILE(COPY ${PROJECT_SOURCE_DIR}/Example_Images/test_input.jpg    DESTINATION ${PROJECT_BINARY_DIR}/bin)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

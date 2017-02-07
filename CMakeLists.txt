#cmake_minimum_required(VERSION 2.8)

set(ROOT ./)
set(DOCOPT_SOURCES ${ROOT}/docopt-0.6.2)

include_directories(${ROOT})

add_library(docopt ${DOCOPT_SOURCES})
set_target_properties(docopt PROPERTIES LINKER_LANGUAGE CXX) # Tell cmake that this is c++ code. This avoids the following error: 'CMake Error: Cannot determine link language for target "docopt". CMake Error: CMake can not determine linker language for target: docopt'

include_directories(${DOCOPT_SOURCES})
#add_subdirectory(${DOCOPT_SOURCES})

#add_library(docopt STATIC IMPORTED)
#set_target_properties(docopt PROPERTIES IMPORTED_LOCATION ${ROOT}/docopt-0.6.2)
#link_directories(${ROOT}/docopt-0.6.2)
#TARGET_LINK_LIBRARIES(main ${ROOT}/docopt-0.6.2)


ADD_EXECUTABLE(main main.cpp marker.cpp page.cpp)

FIND_PACKAGE(OpenCV REQUIRED)

TARGET_LINK_LIBRARIES(main ${OpenCV_LIBS})

set(CMAKE_CXX_FLAGS "-std=c++11") # docopt needs this, per https://github.com/Qihoo360/logkafka/issues/1

FILE(COPY ${PROJECT_SOURCE_DIR}/Example_Images/test_input.jpg    DESTINATION ${PROJECT_BINARY_DIR}/bin)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

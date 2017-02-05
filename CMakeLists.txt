cmake_minimum_required(VERSION 2.8)
 
set(ROOT ./)

####################
# For setting up docopt
####################

include(ExternalProject)

# set(CMAKE_CXX_FLAGS "-std=c++11") # docopt needs this, per https://github.com/Qihoo360/logkafka/issues/1

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )

set(DOCOPT_ROOT ${PROJECT_SOURCE_DIR}/docopt-0.6.2)
set(DOCOPT_INCLUDE_DIRS ${DOCOPT_ROOT}/include/docopt)
set(DOCOPT_LIBRARIES ${DOCOPT_ROOT}/lib/libdocopt.a)
set(docopt_INSTALL_DIR "${DOCOPT_ROOT}")
set(docopt_CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${docopt_INSTALL_DIR})

ExternalProject_Add(docopt
  PREFIX ${DOCOPT_ROOT}
  GIT_REPOSITORY https://github.com/docopt/docopt.cpp.git
  BINARY_DIR ${DOCOPT_ROOT}
  INSTALL_DIR ${DOCOPT_ROOT}
  CMAKE_ARGS ${docopt_CMAKE_ARGS}
  LOG_DOWNLOAD ON
  LOG_CONFIGURE ON
  LOG_BUILD ON
  LOG_INSTALL ON
)
add_library(libdocopt STATIC IMPORTED)
set_target_properties(libdocopt PROPERTIES IMPORTED_LOCATION ${DOCOPT_LIBRARIES})
add_dependencies(libdocopt docopt)

include_directories(docopt-0.6.2)

####################
 
include_directories(${ROOT})
 
ADD_EXECUTABLE(main main.cpp marker.cpp page.cpp)
 
FIND_PACKAGE(OpenCV REQUIRED)
 
TARGET_LINK_LIBRARIES(main ${OpenCV_LIBS})
 

 
FILE(COPY ${PROJECT_SOURCE_DIR}/Example_Images/test_input.jpg    DESTINATION ${PROJECT_BINARY_DIR}/bin)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

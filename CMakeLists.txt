cmake_minimum_required(VERSION 2.8.3)
project(orsens)

## pkg-config libraries
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(avutil libavutil REQUIRED)
pkg_check_modules(avcodec libavcodec REQUIRED)
pkg_check_modules(swscale libswscale REQUIRED)

###########
## Build ##
###########

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} lib)
LINK_DIRECTORIES(lib)

include_directories(include
)

## Declare a cpp executable
add_executable(orsens_sample_detection samples/detection_sample.cpp)
target_link_libraries(orsens_sample_detection
  orsens_lib
  ${avutil_LIBRARIES}  
  ${avcodec_LIBRARIES}
  ${swscale_LIBRARIES}
  ${OpenCV_LIBS} 
)

#############
## Install ##
#############

## Mark executables and/or libraries for installation
#install(TARGETS orsens_lib orsens_sample_detection
#  RUNTIME DESTINATION bin
#  LIBRARY DESTINATION lib
#  ARCHIVE DESTINATION lib
#)

#install(DIRECTORY include/${PROJECT_NAME}/
 #  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
  # FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
#)

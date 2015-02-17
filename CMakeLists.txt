cmake_minimum_required(VERSION 2.8.3)
project(orsens)

## pkg-config libraries
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
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
  orsens
  ${avutil_LIBRARIES}  
  ${avcodec_LIBRARIES}
  ${swscale_LIBRARIES}
  ${OpenCV_LIBS} 
)

add_executable(orsens_sample_segmentation samples/segmentation_sample.cpp)
target_link_libraries(orsens_sample_segmentation
  orsens
  ${avutil_LIBRARIES}  
  ${avcodec_LIBRARIES}
  ${swscale_LIBRARIES}
  ${OpenCV_LIBS} 
)

add_executable(orsens_sample_viewer samples/viewer_sample.cpp)
target_link_libraries(orsens_sample_viewer
  orsens
  ${avutil_LIBRARIES}  
  ${avcodec_LIBRARIES}
  ${swscale_LIBRARIES}
  ${OpenCV_LIBS} 
  ${OPENGL_LIBRARIES}
  ${GLUT_LIBRARY}
)


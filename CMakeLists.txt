cmake_minimum_required(VERSION 2.8.3)
project(orsens)

## pkg-config libraries
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(OpenGL)
find_package(GLUT)
find_package(Qt5 COMPONENTS Core Widgets)
pkg_check_modules(avutil libavutil REQUIRED)
pkg_check_modules(avcodec libavcodec REQUIRED)
pkg_check_modules(swscale libswscale REQUIRED)

###########
## Build ##
###########

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} lib)
set(LIBRARIES_LIST ${avutil_LIBRARIES} ${avcodec_LIBRARIES} ${swscale_LIBRARIES} ${OpenCV_LIBS}) 

LINK_DIRECTORIES(lib)

include_directories(include
)

find_library(OPENBR
    NAMES openbr
    HINTS "lib"
)

## Declare a cpp executable
add_executable(orsens_sample_detection samples/detection_sample.cpp)
target_link_libraries(orsens_sample_detection
  orsens
  ${LIBRARIES_LIST}  
)

add_executable(orsens_sample_segmentation samples/segmentation_sample.cpp)
target_link_libraries(orsens_sample_segmentation
  orsens
  ${LIBRARIES_LIST}  
)

add_executable(orsens_sample_viewer samples/viewer_sample.cpp)
target_link_libraries(orsens_sample_viewer
  orsens
  ${LIBRARIES_LIST}
)

add_executable(orsens_sample_vr samples/vr_sample.cpp)
target_link_libraries(orsens_sample_vr
  orsens
  ${LIBRARIES_LIST}  
)

add_executable(orsens_sample_pointcloud samples/pointcloud_sample.cpp)
target_link_libraries(orsens_sample_pointcloud
  orsens
  ${LIBRARIES_LIST}
  ${OPENGL_LIBRARIES}
  ${GLUT_LIBRARY}
)

add_executable(orsens_sample_biometrics samples/biometrics_sample.cpp)
target_link_libraries(orsens_sample_biometrics
  orsens
  orsens_biometrics
  "${OPENBR}"
  ${LIBRARIES_LIST} 
  ${Qt5Core_LIBRARIES}
)

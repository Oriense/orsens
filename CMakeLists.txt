cmake_minimum_required(VERSION 2.8.3)
project(orsens)

option(WITH_BIOMETRICS "Build biometrics lib and samples" OFF)
option(WITH_OPENGL "Build OpenGL-based samples" ON)

## pkg-config libraries
find_package(PkgConfig REQUIRED)
find_package(OpenCV REQUIRED)
if(WITH_OPENGL)
find_package(OpenGL)
find_package(GLUT)
endif()
if(WITH_BIOMETRICS)
  find_package(Qt5 COMPONENTS Core Widgets)
endif()

pkg_check_modules(avutil libavutil REQUIRED)
pkg_check_modules(avcodec libavcodec REQUIRED)
pkg_check_modules(swscale libswscale REQUIRED)

###########
## Build ##
###########

if(WITH_BIOMETRICS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

if (CMAKE_SYSTEM_PROCESSOR MATCHES ".*64" OR X86_64)
  message("x64")
  set(BIN_DIRECTORY bin/x64)
  set(LIB_DIRECTORY lib/x64)
else()
  set(BIN_DIRECTORY bin)
  set(LIB_DIRECTORY lib)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIRECTORY})
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${LIB_DIRECTORY})
set(LIBRARIES_LIST ${avutil_LIBRARIES} ${avcodec_LIBRARIES} ${swscale_LIBRARIES} ${OpenCV_LIBS}) 

include_directories(include
)

find_library(OPENBR
    NAMES openbr
    HINTS "${LIB_DIRECTORY}"
)

find_library(ORSENS
    NAMES orsens
    HINTS "${LIB_DIRECTORY}"
)

find_library(ORSENS_BIOMETRICS
    NAMES orsens_biometrics
    HINTS "${LIB_DIRECTORY}"
)

## Declare a cpp executable
add_executable(orsens_sample_detection samples/detection_sample.cpp)
target_link_libraries(orsens_sample_detection
  "${ORSENS}"
  ${LIBRARIES_LIST}  
)

add_executable(orsens_sample_segmentation samples/segmentation_sample.cpp)
target_link_libraries(orsens_sample_segmentation
  "${ORSENS}"
  ${LIBRARIES_LIST}  
)

add_executable(orsens_sample_viewer samples/viewer_sample.cpp)
target_link_libraries(orsens_sample_viewer
  "${ORSENS}"
  ${LIBRARIES_LIST}
)

add_executable(orsens_sample_vr samples/vr_sample.cpp)
target_link_libraries(orsens_sample_vr
  "${ORSENS}"
  ${LIBRARIES_LIST}  
)

if(WITH_OPENGL)
add_executable(orsens_sample_pointcloud samples/pointcloud_sample.cpp)
target_link_libraries(orsens_sample_pointcloud
  "${ORSENS}"
  ${LIBRARIES_LIST}
  ${OPENGL_LIBRARIES}
  ${GLUT_LIBRARY}
)
endif()

if(WITH_BIOMETRICS)
add_executable(orsens_sample_biometrics samples/biometrics_sample.cpp)
target_link_libraries(orsens_sample_biometrics
  "${ORSENS}"
  "${ORSENS_BIOMETRICS}"
  "${OPENBR}"
  ${LIBRARIES_LIST} 
  ${Qt5Core_LIBRARIES}
)
endif()

cmake_minimum_required(VERSION 2.8.3)
project(orsens)

option(WITH_BIOMETRICS "Build biometrics lib and samples" OFF)
option(WITH_AR "Build AR lib and samples" ON)
option(WITH_OPENGL "Build OpenGL based samples. Turn on just if you have OpenCV with OpenGL support" OFF)

## pkg-config libraries
find_package(PkgConfig REQUIRED)
find_package(OpenCV REQUIRED)
if(WITH_OPENGL)
find_package(OpenGL QUIET)
find_package(GLUT QUIET)
endif()
if(WITH_BIOMETRICS)
  find_package(Qt5 QUIET COMPONENTS Core)
endif()

if(Qt5Core_INCLUDE_DIRS AND Qt5Core_LIBRARIES)
  set(QT5_FOUND true)
else()
  set(QT5_FOUND false)
endif()

if (QT5_FOUND)
   message(STATUS "Found Qt5, will build with biometrics lib")
else()
   message(STATUS "Could not locate Qt5, biometrics functionlaity is off.")
   set(WITH_BIOMETRICS OFF)
endif()

pkg_check_modules(avutil libavutil REQUIRED)
pkg_check_modules(avcodec libavcodec REQUIRED)
pkg_check_modules(swscale libswscale REQUIRED)

###########
## Build ##
###########

if(WITH_BIOMETRICS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

if(MSVC64 OR MINGW64)
  set(X86_64 1)
elseif(MINGW OR (MSVC AND NOT CMAKE_CROSSCOMPILING))
  set(X86 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "amd64.*|x86_64.*|AMD64.*")
  set(X86_64 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "i686.*|i386.*|x86.*|amd64.*|AMD64.*")
  set(X86 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm.*|ARM.*)")
  set(ARM 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64.*|AARCH64.*)")
  set(AARCH64 1)
endif()

if (X86_64)
  message("Architecture x64")
  set(BIN_DIRECTORY bin/x64)
  set(LIB_DIRECTORY lib/x64)
elseif (ARM)
  message("Architecture ARM")
  set(BIN_DIRECTORY bin/arm)
  set(LIB_DIRECTORY lib/arm)
elseif(X86)
  message("Architecture x86")
  set(BIN_DIRECTORY bin)
  set(LIB_DIRECTORY lib)
endif()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIRECTORY})
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${LIB_DIRECTORY})

set(LIBRARIES_LIST ${avutil_LIBRARIES} ${avcodec_LIBRARIES} ${swscale_LIBRARIES} ${OpenCV_LIBS}) 

include_directories(include
)

find_library(ORSENS
    NAMES orsens
    HINTS "${LIB_DIRECTORY}"
)

if(WITH_BIOMETRICS)
find_library(OPENBR
    NAMES openbr
    HINTS "${LIB_DIRECTORY}"
)
find_library(ORSENS_BIOMETRICS
    NAMES orsens_biometrics
    HINTS "${LIB_DIRECTORY}"
)
endif()

if(WITH_AR)
find_library(ARUCO
    NAMES aruco
    HINTS "${LIB_DIRECTORY}"
)
find_library(ORSENS_AR
    NAMES orsens_ar
    HINTS "${LIB_DIRECTORY}"
)
endif()

if (ARUCO AND ORSENS_AR)
  message(STATUS "Found AR libs.")
else()
  message(STATUS "AR libs not found, AR functionality is off.")
  set(WITH_AR OFF)
endif()

add_executable(orsens_sample_viewer samples/viewer_sample.cpp)
target_link_libraries(orsens_sample_viewer
  "${ORSENS}"
  ${LIBRARIES_LIST}
)

add_executable(orsens_sample_obstacle_avoidance samples/obstacle_avoidance_sample.cpp)
target_link_libraries(orsens_sample_obstacle_avoidance
  "${ORSENS}"
  ${LIBRARIES_LIST} 
)

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

add_executable(orsens_sample_vr samples/vr_sample.cpp)
target_link_libraries(orsens_sample_vr
  "${ORSENS}"
  ${LIBRARIES_LIST}  
)

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

if(WITH_AR AND ARUCO AND ORSENS_AR)
add_executable(orsens_sample_ar samples/ar_sample.cpp)
target_link_libraries(orsens_sample_ar
  "${ORSENS}"
  "${ORSENS_AR}"
  "${ARUCO}"
  ${LIBRARIES_LIST} 
)
endif()

if(WITH_OPENGL)
add_executable(orsens_sample_pointcloud samples/pointcloud_sample.cpp)
target_link_libraries(orsens_sample_pointcloud
  "${ORSENS}"
  ${LIBRARIES_LIST}
  ${OPENGL_LIBRARIES}
  ${GLUT_LIBRARY}
)
endif()

# SDK for OrSens 3d-camera
ROS package is [here](https://github.com/Oriense/orsens_ros).
## Overview
[bin](https://github.com/Oriense/orsens/tree/master/bin) contains prebuilt examples of the functionality.

[lib](https://github.com/Oriense/orsens/tree/master/lib) - core libraries of the SDK, as well as some 3rd-parties. [x86](https://github.com/Oriense/orsens/tree/master/lib), [x64](https://github.com/Oriense/orsens/tree/master/lib/x64) and [arm](https://github.com/Oriense/orsens/tree/master/lib/arm) versions available. Compiled with GCC and Visual Studio 2010.

[data](https://github.com/Oriense/orsens/tree/master/data) - files needed for some functions: calibration data, trained classifiers etc.

[samples](https://github.com/Oriense/orsens/tree/master/samples)  - source code of the examples. 

Example of running samples and ROS nodes - http://www.youtube.com/watch?v=91DyxcnwOBM.

## Build
For Linux use [CMake](https://github.com/Oriense/orsens/tree/master/CMakeLists.txt), for Windows - Visual Studio [solution](https://github.com/Oriense/orsens/tree/master/samples/vs_solution).

#####Supported Platforms
Currently tested on:
* Windows 7, 8  (32-bit and 64-bit)
* Ubuntu 12.04, 14.04 (32-bit and 64-bit)
* Lubuntu 14.04 (armv7)
 
#####Hardware requirements
* USB port, better 3.0
* 1GB RAM

Currently tested on:
* Intel Core i5 laptops
* Odroid U3 single-board computer (quad core ARM processor, 1GB RAM)

#####Dependencies. 
For Windows all dependencies are provided.

For Linux, mandatory: libopencv-dev libavutil-dev libavcodec-dev libswscale-dev 

Optional:
* OpenGL and it's support in OpenCV, GLUT - for visualization in some samples (use WITH_OPENGL cmake option to turn in on/off, default - on)
* OpenBR (provided) and Qt5 - for biometrics functionality (WITH_BIOMETRICS, default - off)

## Functionality
Camera itself provides depth and rgb stream via USB, and SDK contains set of ready-to-use computer vision functions. 

Currenlty implemented:
- [Image and depthmap processing, measuring](https://github.com/Oriense/orsens/blob/master/samples/viewer_sample.cpp)
- [Obstacle detection](https://github.com/Oriense/orsens/blob/master/samples/obstacle_avoidance_sample.cpp)
- [Scene segmentation](https://github.com/Oriense/orsens/blob/master/samples/segmentation_sample.cpp)
- [Face detection](https://github.com/Oriense/orsens/blob/master/samples/detection_sample.cpp) and [recognition](https://github.com/Oriense/orsens/blob/master/samples/biometrics_sample.cpp)

Coming soon:
- Integration with SLAM implementations
- Tracking
- AR, VR
- and much more

## Troubleshooting 
### 1. Capturing
* **"Select timeout" or "Cannot allocate memory" messages**

 It's a problem of USB bandwidth, you can read more here - http://www.ideasonboard.org/uvc/faq/

 Try to tune uvcvideo:
 ```
 sudo rmmod uvcvideo
 sudo modprobe uvcvideo quirks=0x80 (or nodrop=1)
 ```

 To make this permanent - edit and copy [uvcvideo.conf](https://github.com/Oriense/orsens/blob/master/uvcvideo.conf) to /etc/modprobe.d
 
 http://www.youtube.com/watch?v=ynnW0aibcI4

* **pPin is NULL!**

 It means that camera is not detected
 
 Check connection or try to remove any other webcams  (or disable integrated one)

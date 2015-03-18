# SDK for OrSens 3d-camera

**bin** directory contains prebuilt examples, **samples** directory - source code of the examples: you can use CMake or Visual Studio (samples/vs_solution) to build it

ROS users can clone the [driver](https://github.com/Oriense/orsens_ros) and build with catkin

Example of running samples and ROS nodes - http://www.youtube.com/watch?v=91DyxcnwOBM

## Functionality
Camera itself provides depth and rgb stream via USB, and SDK contains set of ready-to-use computer vision functions 

Currenlty implemented:
- Windows and Linux support (Ubuntu), [ROS driver](https://github.com/Oriense/orsens_ros)
- Image and depthmap processing
- Measuring 
- Obstacle detection
- Scene segmentation
- Face detection and recognition

Coming soon
- Integration with SLAM implementations
- Tracking
- and much more

## Troubleshooting 
"Select timeout" or "Cannot allocate memory" errors - it's a problem of USB bandwidth, you can read more here - http://www.ideasonboard.org/uvc/faq/

Try to tune uvcvideo:
```
sudo rmmod uvcvideo
sudo modprobe uvcvideo quirks=0x80 
```
or 
```
sudo modprobe uvcvideo nodrop=1
```
http://www.youtube.com/watch?v=ynnW0aibcI4

To make this permanent - edit and copy [uvcvideo.conf](https://github.com/Oriense/orsens/blob/master/uvcvideo.conf) to /etc/modprobe.d

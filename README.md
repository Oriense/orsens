# SDK for OrSens 3d-camera

**bin** directory contains prebuilt examples, **samples** directory - source code of the examples: you can use CMake or Visual Studio (samples/vs_solution) to build it

ROS users can clone the [driver](https://github.com/Oriense/orsens_ros) and build with catkin

## Functionality
Camera itself provides depth and rgb stream via USB, and SDK contains set of ready-to-use computer vision functions 

Currenlty implemented:
- Windows and Linux support, [ROS driver](https://github.com/Oriense/orsens_ros)
- Depthmap filtering
- Measuring 
- Obstacle detection
- Scene segmentation: floor
- Object detection: faces

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

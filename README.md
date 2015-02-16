# orsens
SDK for OrSens 3d-camera

Currenlty implemented:
- Windows and Linux support, [ROS driver](https://github.com/Oriense/orsens_ros)
- Depthmap filtering
- Measuring 
- Detection: faces

Coming soon
- Integration with SLAM implementations
- Scene segmentation
- Obstacle detection and avoidance
- Tracking
- and much more

# usage
**bin** directory contains prebuilt examples, **samples** directory - source code, you can use CMake or Visual Studio solution (samples/vs_solution) to build it

ROS users can clone the [driver](https://github.com/Oriense/orsens_ros) and build with catkin

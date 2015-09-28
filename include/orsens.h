/**
Commercial use of this software is allowed only with conjunction with hardware made by Oriense company (PROvision LLC)

In the rest under the MIT License (MIT)

Copyright (c) 2015 Oriense (PROvision LLC)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 \mainpage SDK for OrSens 3d-camera
 \section Functionality
Camera itself provides depth and rgb stream via USB, and SDK contains set of ready-to-use computer vision functions.
 - Currenlty implemented:
   - Image and depthmap processing
   - Measuring
   - Obstacle detection
   - Scene segmentation
   - Human face detection and recognition
 - Coming soon:
   - Integration with SLAM implementations
   - Tracking
   - AR, VR
   - and much more
 \section Build
<a href="https://github.com/Oriense/orsens">This</a> repository contains prebuilt orsens library with samples.

For building samples for Linux use CMake, for Windows - Visual Studio solution.

<a href="https://github.com/Oriense/orsens_ros">This</a> repository contains ROS package for OrSens. For building you can use catkin as usual.



\subsection Supported Supported Platforms

Currently tested on:

   - Windows 7, 8 (32-bit and 64-bit)
   - Ubuntu 12.04, 14.04 (32-bit and 64-bit)
   - Lubuntu 14.04 (armv7)

\subsection Hardware Hardware requirements

   - USB port, better 3.0
   - 1GB RAM

Currently tested on:

   - Intel Core i5 laptops
   - Odroid U3 single-board computer (quad core ARM processor, 1GB RAM)

\subsection Dependencies Dependencies

For Windows all dependencies are provided.

For Linux, mandatory: libopencv-dev libavutil-dev libavcodec-dev libswscale-dev

Optional:

   - OpenGL and it's support in OpenCV, GLUT - for visualization in some samples (use WITH_OPENGL cmake option to turn in on/off, default - on)
   - OpenBR (provided) and Qt5 - for biometrics functionality (WITH_BIOMETRICS, default - off)


**/
#ifndef ORSENS_H_INCLUDED
#define ORSENS_H_INCLUDED

#define NOMINMAX

#include <stdint.h>
#include <stdio.h>

#include "orcv.h"
#include "orar.h"

/**
\brief structure for point with 2D image and 3D world coordinates
*/
struct ScenePoint
{
    Point2i pt_image;
    Point3f pt_world;
};

/**
\brief general structure for all objects
*/
struct SceneObject
{
    /**distance from camera to object (mm)*/
    uint16_t dist;
    /**angle between camera's and object's central point (-180 180)*/
    double angle;

    /**object's bounding rectagnle*/
    Rect bounding_rect;
    /**object's centre in image coordinatas*/
    Point2i centre;
    /**object's centre in world coordinates*/
    Point3f centre_world;
};

/**
\brief structure for humans with biometrics
*/
struct Human : SceneObject
{
    /** it can be "Male", "Female" and "Not sure" */
    string gender;
    /** approximate age */
    uint8_t age;

    /** human's face bounding rectangle */
    Rect face_rect;
    /** left eye coordinates */
    Point left_eye_pos;
    /** right eye coordinates */
    Point right_eye_pos;
};

/**
\brief structure for obstacles, wich contains information about the nearest and furthest point
*/
struct Obstacle : SceneObject
{
    /** nearest point */
    Point3f min_pt_world;
    /** furthest point */
    Point3f max_pt_world;
};

/**
\brief structure for all scene info
*/
struct SceneInfo
{
    /** nearest point of the scene */
    uint16_t nearest_distance;
    /** nearest disparity of the scene */
    uint8_t nearest_disp;
    /** nearest point of the scene */
    ScenePoint nearest_point;
    /** nearest obstacle of the scene */
    Obstacle nearest_obstacle;
};

/**
\brief Main class for orsens camera
*/
class Orsens
{

private:

    static const int NO_DISTANCE = 0;
    static const int NO_ANGLE = 361;

    string data_path_;

    uint16_t color_width_;
    uint16_t color_height_;
    uint8_t color_rate_;
    uint16_t depth_width_;
    uint16_t depth_height_;
    uint8_t depth_rate_;

    uint16_t left_width_;
    uint16_t left_height_;

    //images
    Mat left_, right_;
    Mat left_gray_, right_gray_;
    Mat disp_, disp_raw_, disp_raw_prev_, disp_src_, depth_, depth8_;
    Mat point_cloud_;
    Mat segmentation_mask_;

    //what we've got processed
    bool got_gray_, got_depth_, got_seg_mask;

    //parametrs
    Rect roi_;

    //camera info
    static const int DISPARITY_COUNT = 256;
    uint16_t zdtable_[DISPARITY_COUNT];

    uint16_t min_distance_;
    uint16_t max_distance_;
    uint16_t cx_;
    uint16_t cy_;
    uint16_t baseline_;
    uint16_t focal_;
    float fov_;

    //scene info
    std::vector<Human> humans_;
    SceneInfo scene_info_;

    //processing
    bool makeGray();
    bool makeDepth();
    bool makeNearestDistance();
    bool makeNearestPoint();
    bool makeNearestObstacle();
    bool segmentFloor();

public:
    /** empty constructor */
    Orsens() {};
    ~Orsens() {};

    typedef enum
    {
        CAPTURE_DEPTH_ONLY=0, CAPTURE_LEFT_ONLY, CAPTURE_DEPTH_LEFT, CAPTURE_LEFT_RIGHT,
    } CaptureMode;

    /** capture mode: only depth image, only left image, depth and left images or left and right images*/
    CaptureMode capture_mode_;

    /**
    * use this if you want to set the value of CaptureMode in ros launch

    * possible values: depth_only, left_only, depth_left and left_right
    */
    static CaptureMode captureModeFromString(const std::string& str);

    /** start capturing */
    bool start(CaptureMode capture_mode=CAPTURE_DEPTH_ONLY, string data_path="../../data", uint16_t color_width=640, uint16_t depth_width=640, uint8_t color_rate=15, uint8_t depth_rate=15,
               bool compress_color=false, bool compress_depth=false, float fov=60.0, uint16_t baseline=60);
    /** stop capturing */
    bool stop();

    //setting parametrs

    /**sets region of interest
    * @param Rect region of intrest; default: whole image
    */
    void setRoi(Rect);

    /** grab current images from sensor */
    bool grabSensorData();

    //processing
    /**removes speckles
    * @param maxSpeckleSize The maximum speckle size to consider it a speckle
    */
    bool filterDisp(uint16_t maxSpeckleSize=1000);

    /**removes floor points
    */
    bool removeFloor();

    //getting data

   /** returns rgb image from left camera
   */
    Mat getLeft();
   /** returns rgb image from right camera
   */
    Mat getRight();
   /** returns disparity map
    * @param colored if set disp is colored for visualization
   */
    Mat getDisp(bool colored=false);
   /** returns depth map
    * @param depth8 if set depth is scaled to fit 8 bit for visualizaton
   */
    Mat getDepth(bool depth8=false);
   /** returns point cloud
   */
    Mat getPointCloud();

    /** returns size of the depth image
    */
    Size getDepthSize();
    /** returns size of the left image
    */
    Size getColorSize();
    /** returns depth rate */
    uint8_t getRate();

    //measuring. distances are in millimetres, except functions ends with M (metres). directions in angles
    /** returns disparity at image point */
    uint8_t disparityAtImagePoint(uint16_t x, uint16_t y);
    /** returns distance to image point in millimetres */
    uint16_t distanceToImagePoint(uint16_t x, uint16_t y);
    /** returns distance to image rectangle in millimetres */
    uint16_t distanceToImageRect(Rect rect);
    /** returns world point at image point in millimetres */
    Point3i worldPointAtImagePoint(uint16_t x, uint16_t y);
    /** returns world point at image point in metres */
    Point3f worldPointAtImagePointM(uint16_t x, uint16_t y);
    /** returs directon to image point*/
    float directionToImagePoint(uint16_t x, uint16_t y);
    /** returns direction to image rectangle */
    float directionToImageRect(Rect rect);

    /** returns minimun possible distance camera able to measure
    */
    uint16_t getMinDistance();
    /** returns maximum possible distance
    */
    uint16_t getMaxDistance();

    //scene information
    /** returns nearest distance, nearest point(if nearest_point is set to true) and nearest obstacle(if nearest_obstacle is set to true)
     * @param nearest_obstacle if set estimates nearest obstacle information
     * @param nearest_point if set estimates nearest point information
    */
    SceneInfo getSceneInfo(bool nearest_obstacle=true, bool nearest_point=true);
    /** returns farest point
     * @param width x-zone width
    */
    ScenePoint getFarestPoint(uint16_t width=100);
    /** returns grid with given cols and rows
     * @param cols vector of column widths
     * @param rows vector of row widths
     * @param dist_th distance threshold
     * @param occ_th occupancy threshold
     * @param pts_th minimun number of points with distances less than dist_th in % in each cell to consider it as filled
    */
    Mat getSceneGrid(vector<float> cols, vector<float> rows, int dist_th=3000, float occ_th=0.1, float pts_th=0.01);

    //detection
    /** returns vector of all humans in the scene
    */
    std::vector<Human> getHumans();
    /** estimates age and gender of nearest human
     * @param human output value
    */
    bool getNearestHumanBiometrics(Human &human);

    //aruco functions
    /**
    returns aruco markers, for more information see ArUco documentation
    * @param camera_num 0 - left camera, 1 - right camera
    * @param marker_size size of the marker side expressed in meters
    */
    vector<Marker> getMarkers(uint8_t camera_num=0, float marker_size=0.04);
    /** returns camera parametres used by aruco
    * @param camera_num 0 - left camera, 1 - right camera
    */
    CameraParameters getARCameraParametres(uint8_t camera_num);

    //misc
    /** returns rgb value for given distance
    * @param dist distance in millimetres
    */
    Scalar dist2rgb(uint16_t dist);
    /** returns diparity for given distance
    * @param dist distance in millimetres
    */
    uint8_t dist2disp(uint16_t dist);
    /** returns distance for given disparity
    * @param disp disparity (0-255)
    */
    uint16_t disp2dist(uint8_t disp);
};

#endif // ORSENS_H_INCLUDED

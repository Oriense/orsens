#ifndef ORSENS_H_INCLUDED
#define ORSENS_H_INCLUDED

#define NOMINMAX

#include <stdint.h>
#include <stdio.h>

#include "orcv.h"
#include "orar.h"

struct ScenePoint
{
    Point2i pt_image;
    Point3f pt_world;
};

//general structure for all objects
struct SceneObject
{
    uint16_t dist; // distance from camera to object (mm)
    double angle; //between camera's and object's central point (-180 180)

    Rect bounding_rect;
    Point2i centre;
    Point3f centre_world;
};

struct Human : SceneObject
{
    string gender;
    uint8_t age;

    Rect face_rect;
    Point left_eye_pos;
    Point right_eye_pos;
};

struct Obstacle : SceneObject
{
    Point3f min_pt_world;
    Point3f max_pt_world;
};

struct SceneInfo
{
    uint16_t nearest_distance;
    uint8_t nearest_disp;
    ScenePoint nearest_point;
    Obstacle nearest_obstacle;
};

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
    Orsens() {};
    ~Orsens() {};

    typedef enum
    {
        CAPTURE_DEPTH_ONLY=0, CAPTURE_LEFT_ONLY, CAPTURE_DEPTH_LEFT, CAPTURE_LEFT_RIGHT,
    } CaptureMode;

    CaptureMode capture_mode_;

    static CaptureMode captureModeFromString(const std::string& str);

    bool start(CaptureMode capture_mode=CAPTURE_DEPTH_ONLY, string data_path="../../data", uint16_t color_width=640, uint16_t depth_width=640, uint8_t color_rate=15, uint8_t depth_rate=15,
               bool compress_color=false, bool compress_depth=false, float fov=60.0, uint16_t baseline=60);
    bool stop();

    //setting parametrs
    void setRoi(Rect); //set region of interest, default: whole image

    bool grabSensorData();

    //processing
    bool filterDisp(uint16_t maxSpeckleSize=1000); //filter speckles
    bool removeFloor(); //remove floor points

    //getting data
    Mat getLeft();
    Mat getRight();
    Mat getDisp(bool colored=false); //if colored is set to true, then disp is colored for visualization
    Mat getDepth(bool depth8=false); //if depth8 is set to true, then depth is scaled to fit 8 bit for visualizaton
    Mat getPointCloud();

    Size getDepthSize();
    Size getColorSize();
    uint8_t getRate();

    //measuring. distances are in millimetres, except functions ends with M (metres). directions in angles
    uint8_t disparityAtImagePoint(uint16_t x, uint16_t y);
    uint16_t distanceToImagePoint(uint16_t x, uint16_t y);
    uint16_t distanceToImageRect(Rect rect);
    Point3i worldPointAtImagePoint(uint16_t x, uint16_t y);
    Point3f worldPointAtImagePointM(uint16_t x, uint16_t y);
    float directionToImagePoint(uint16_t x, uint16_t y);
    float directionToImageRect(Rect rect);

    uint16_t getMinDistance(); //minimun possible distance camera able to measure
    uint16_t getMaxDistance(); //maximum possible distance

    //scene information
    SceneInfo getSceneInfo(bool nearest_obstacle=true, bool nearest_point=true); //get nearest distance, nearest point(if nearest_point is set to true) and nearest obstacle(if nearest_obstacle is set to true)
    ScenePoint getFarestPoint(uint16_t width=100); //finds farest point with given x-zone width
    Mat getSceneGrid(vector<float> cols, vector<float> rows, int dist_th=3000, float occ_th=0.1, float pts_th=0.01); //get grid with given cols and rows

    //detection
    std::vector<Human> getHumans();
    bool getNearestHumanBiometrics(Human &human); //estimates age and gender of nearest human

    //aruco functions
    vector<Marker> getMarkers(uint8_t camera_num=0, float marker_size=0.04);
    CameraParameters getARCameraParametres(uint8_t camera_num);

    //misc
    Scalar dist2rgb(uint16_t dist);
    uint8_t dist2disp(uint16_t dist);
    uint16_t disp2dist(uint8_t disp);
};

#endif // ORSENS_H_INCLUDED

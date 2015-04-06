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
    float discrete_depth_step_;

    uint16_t left_width_;
    uint16_t left_height_;

    //images
    Mat left_, right_;
    Mat left_gray_, right_gray_;
    Mat disp_, disp_raw_, disp_raw_prev_, depth_, depth8_, discrete_depth_, discrete_depth8_;
    Mat point_cloud_;
    Mat segmentation_mask_;

    //what we've got processed
    bool got_gray_, got_depth_, got_discrete_depth_;

    //camera info
    static const int DISPARITY_COUNT = 256;
    uint16_t zdtable_[DISPARITY_COUNT];

    uint16_t min_distance_;
    uint16_t max_distance_;
    uint16_t cx_;
    uint16_t cy_;
    uint16_t baseline_;
    uint16_t focal_;

    //scene info
    std::vector<Human> humans_;

    //processing
    bool makeGray();
    bool makeDepth();
    bool makeDiscreteDepth();
    bool segmentFloor(Mat disp);

public:
    Orsens() {};
    ~Orsens() {};

    typedef enum
    {
        CAPTURE_DEPTH_ONLY=0, CAPTURE_LEFT_ONLY, CAPTURE_DEPTH_LEFT, CAPTURE_LEFT_RIGHT,
    } CaptureMode;

    CaptureMode capture_mode_;

    static CaptureMode captureModeFromString(const std::string& str);

    bool start(CaptureMode capture_mode=CAPTURE_DEPTH_ONLY, string data_path="../data", uint16_t color_width=640, uint16_t depth_width=640, uint8_t color_rate=15, uint8_t depth_rate=15,
               bool compress_color=false, bool compress_depth=false, uint16_t baseline=60);
    bool stop();

    bool initBiometrics(bool init_gender_estimation=true, bool init_age_estimation=false);
    void deinitBiometrics();

    bool grabSensorData();

    //setting parametrs
    void setDiscreteDepthStep(float step);

    //getting data
    Mat getLeft();
    Mat getRight();
    Mat getDisp();
    Mat getDispColored();
    Mat getDepth();
    Mat getDepth8(); //scaled to fit 8 bit
    Mat getDiscreteDepth();
    Mat getPointCloud();
    Mat getDiscreteDepth8();
    Mat getSegmentationMask();

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

    uint16_t getNearestDistance(Rect roi=Rect()); // finds nearest distance in the region, if roi is empty - in a whole image
    ScenePoint getNearestPoint(Rect roi=Rect()); // the same, but point
    Obstacle getNearestObstacle(Rect roi=Rect());

    ScenePoint getFarestPoint(uint16_t width=100, Rect roi=Rect()); //finds farest point with given x-zone width in the region, if roi is empty - in a whole image

    //detection
    std::vector<Human> getHumans();
    bool getNearestHumanBiometrics(Human &human);

    //ar
    bool initAR(float marker_size=0.04);
    void deinitAR();
    vector<Marker> getMarkers(uint8_t camera_num=0);
    CameraParameters getARCameraParametres(uint8_t camera_num);

    //misc
    Scalar dist2rgb(uint16_t dist);
    uint8_t dist2disp(uint16_t dist);

};

#endif // ORSENS_H_INCLUDED

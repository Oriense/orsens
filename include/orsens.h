#ifndef ORSENS_H_INCLUDED
#define ORSENS_H_INCLUDED

#define NOMINMAX

#include <stdint.h>

#include "orcv.h"
#include "EtronDI_O.h"

const int DISPARITY_COUNT = 256;
const int NO_DISTANCE = 0;
const int TOO_CLOSE_DISTANCE = 10000;
const int NO_ANGLE = 361;

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

};

class Orsens
{

private:

    int	color_width_;
    int	color_height_;
    int	depth_width_;
    int	depth_height_;

    int	image_buf_size_;

    //buffers for data from sensor
    uint8_t	*image_buf;
    uint8_t	*color_buf;
    uint8_t	*depth_buf;

    //images
    Mat left_, right_;
    Mat left_gray_, right_gray_;
    Mat disp_, disp_raw_, disp_raw_prev_, depth_;

    //what we've got processed
    bool got_gray_;

    //camera info
    uint16_t zdtable_[DISPARITY_COUNT];
    TABLEINFO tableinfo_;
    uint16_t min_distance_;
    uint16_t max_distance_;
    uint16_t cx_;
    uint16_t cy_;
    uint16_t baseline_;
    uint16_t focal_;

    //scene info
    std::vector<Human> humans_;

    //capturing
    bool allocImageBuffers(int cx1, int cy1, int cx2, int cy2);
    void freeImageBuffers();

    //processing
    bool makeGray();

public:
    Orsens(){};
    ~Orsens(){};

    bool start(uint16_t baseline);
    bool stop();
    bool grabSensorData();

    //getting data
    Mat getLeft();
    Mat getDispColored();

    //measuring. distances are in millimetres, except functions ends with M (metres). directions in angles
    uint8_t disparityAtImagePoint(uint16_t x, uint16_t y);
    uint16_t distanceToImagePoint(uint16_t x, uint16_t y);
    uint16_t distanceToImageRect(Rect rect);
    Point3i worldPointAtImagePoint(uint16_t x, uint16_t y);
    Point3f worldPointAtImagePointM(uint16_t x, uint16_t y);
    float directionToImagePoint(uint16_t x, uint16_t y);
    float directionToImageRect(Rect rect);

    //detection
    std::vector<Human> getHumans();

      //misc
    Scalar dist2rgb(uint16_t dist);
    uint8_t dist2disp(uint16_t dist);

};

#endif // ORSENS_H_INCLUDED

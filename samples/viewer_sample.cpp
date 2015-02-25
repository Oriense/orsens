#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

#include "orsens.h"

const String left_window_name = "left";
const String right_window_name = "right";
const String depth_window_name = "depth";

using namespace std;

Orsens orsens;

static void printCommandLineParams()
{
    cout << "-mode     capture mode: 0 - depth only, 1 - left only, 2 - depth and left, 3 - left and right" << endl;
    cout << "-cmp      Compress stream (mjpeg)" << endl;
    cout << "-data     Path to data folder" << endl ;
    cout << "-dw       Depth image width" << endl ;
    cout << "-cw       Color image width" << endl ;
    cout << "-dr       Depth stream rate" << endl ;
    cout << "-cr       Color stream rate" << endl ;
}


void onMouse(int event, int x, int y, int flags, void* userdata)
{
    if  ( event == EVENT_LBUTTONDOWN )
    {
        Point3f world_point = orsens.worldPointAtImagePointM(x,y);
        uint8_t disparity = orsens.disparityAtImagePoint(x,y);

        if (world_point.z==0)
            printf("empty point\n");
        else
        {
            float ang = orsens.directionToImagePoint(x,y);
            printf("%d %d: d=%d, z=%4.2f, x=%4.2f y=%4.2f, ang=%4.2f\n", x, y, disparity, world_point.z, world_point.x, world_point.y, ang);
        }
    }
}

int main( int argc, char **argv )
{
    int mode = Orsens::CAPTURE_DEPTH_LEFT;
    bool compress = false;
    string data_path="../data";
    int color_width=640;
    int depth_width=640;
    int color_rate=15;
    int depth_rate=15;

    for( int i = 1; i < argc; i++ )
    {
        if( !strcmp( argv[i], "--help" ) || !strcmp( argv[i], "-h" ) )
        {
            printCommandLineParams();
            exit(0);
        }
        else if( !strcmp( argv[i], "-mode" ) )
        {
            mode = atoi(argv[++i]);
        }
        else if( !strcmp( argv[i], "-cmp" ) )
        {
            compress = true;
            printf("compress requested\n");
        }
        else if( !strcmp( argv[i], "-dw" ) )
        {
            depth_width = atoi(argv[++i]);
            printf("depth width requested: %d\n", depth_width);
        }
        else if( !strcmp( argv[i], "-cw" ) )
        {
            color_width = atoi(argv[++i]);
            printf("color width requested: %d\n", color_width);
        }
        else if( !strcmp( argv[i], "-dr" ) )
        {
            depth_rate = atoi(argv[++i]);
            printf("depth rate requested: %d\n", depth_rate);
        }
        else if( !strcmp( argv[i], "-cr" ) )
        {
            color_rate = atoi(argv[++i]);
            printf("color rate requested: %d\n", color_rate);
        }
        else if( !strcmp( argv[i], "-data" ) )
        {
            data_path = argv[++i];
            printf("data path: %s\n", data_path.c_str());
        }
    }

    namedWindow(depth_window_name);
    setMouseCallback(depth_window_name, onMouse, NULL);
    namedWindow(left_window_name);
    setMouseCallback(left_window_name, onMouse, NULL);

    switch (mode)
    {
    case Orsens::CAPTURE_DEPTH_ONLY:
        destroyWindow(left_window_name);
        break;

    case Orsens::CAPTURE_LEFT_ONLY:
        destroyWindow(depth_window_name);
        break;

    case Orsens::CAPTURE_LEFT_RIGHT:
        namedWindow(right_window_name);
        break;
    }

    bool colorize_depth = true;

    if (!orsens.start((Orsens::CaptureMode)mode, data_path, color_width, depth_width, color_rate, depth_rate, compress))
    {
        printf("unable to start\n");
        return -1;
    }


    while (true)
    {
        orsens.grabSensorData();

        switch (mode)
        {
        case Orsens::CAPTURE_DEPTH_ONLY:
            if (colorize_depth)
                imshow(depth_window_name, orsens.getDispColored());
            else
                imshow(depth_window_name, orsens.getDisp());
            break;

        case Orsens::CAPTURE_LEFT_ONLY:
            imshow(left_window_name, orsens.getLeft());
            break;

        case Orsens::CAPTURE_DEPTH_LEFT:

            if (colorize_depth)
                imshow(depth_window_name, orsens.getDispColored());
            else
                imshow(depth_window_name, orsens.getDisp());

            imshow(left_window_name, orsens.getLeft());
            break;

        case Orsens::CAPTURE_LEFT_RIGHT:
            imshow(left_window_name, orsens.getLeft());
            imshow(right_window_name, orsens.getRight());
            break;
        }

        char c = (char)waitKey(1000/orsens.getRate());

        switch (c)
        {
        case 'c':
            colorize_depth = !colorize_depth;
            break;
        }

        if (c==27)
            break;

    }

    orsens.stop();

    return 0;
}

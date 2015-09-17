#include "orsens.h"
#include <opencv2/calib3d/calib3d.hpp>

using namespace std;

Orsens orsens;

static void printCommandLineParams()
{
    cout << "-mode     mode: 0 - mono, 1 - stereo" << endl;
    cout << "-size     marker size in metres (default 0.04)" << endl;

}

int main( int argc, char **argv )
{
    uint8_t mode=1;
    float marker_size = 0.04;

    for( int i = 1; i < argc; i++ )
    {
        if( !strcmp( argv[i], "--help" ) || !strcmp( argv[i], "-h" ) )
        {
            printCommandLineParams();
            exit(0);
        }
        else if( !strcmp( argv[i], "-mode" ) )
        {
            mode = atoi(argv[i+1]);
        }
        else if( !strcmp( argv[i], "-size" ) )
        {
            marker_size = atof(argv[i+1]);
        }
    }

    int capture_mode = Orsens::CAPTURE_LEFT_ONLY;

    if (mode==1)
        capture_mode = Orsens::CAPTURE_LEFT_RIGHT;

    if (!orsens.start((Orsens::CaptureMode)capture_mode))
    {
        printf("unable to start\n");
        return -1;
    }

    while (true)
    {
        orsens.grabSensorData();

        vector<Marker> left_markers = orsens.getMarkers(0, marker_size);

        Mat left = orsens.getLeft();

        cout << endl << "left ";

        for (unsigned int i=0; i<left_markers.size(); i++)
        {
            cout<<endl<<left_markers[i];
            Mat Rmat;
            cv::Rodrigues(left_markers[i].Rvec, Rmat);
            cout<<endl << Rmat;

            left_markers[i].draw(left,Scalar(0,0,255),1);

            if (orsens.getARCameraParametres(0).isValid())
            {
                CvDrawingUtils::draw3dCube(left,left_markers[i],orsens.getARCameraParametres(0));
                CvDrawingUtils::draw3dAxis(left,left_markers[i],orsens.getARCameraParametres(0));
            }
        }

        imshow("left", left);

        if (mode==1)
        {
            cout<<endl << "right ";
            vector<Marker> right_markers = orsens.getMarkers(1, marker_size);

            Mat right = orsens.getRight();

            for (unsigned int i=0; i<right_markers.size(); i++)
            {
                cout<<endl<<right_markers[i];
                Mat Rmat_right;
                cv::Rodrigues(right_markers[i].Rvec, Rmat_right);
                cout<<endl << Rmat_right;

                right_markers[i].draw(right,Scalar(0,0,255),1);

                if (orsens.getARCameraParametres(1).isValid())
                {
                    CvDrawingUtils::draw3dCube(right,right_markers[i],orsens.getARCameraParametres(1));
                    CvDrawingUtils::draw3dAxis(right,right_markers[i],orsens.getARCameraParametres(1));
                }
            }

            imshow("right", right);

        }

        char c = waitKey(1000/orsens.getRate());

        if (c==27)
            break;


    }

    orsens.stop();

    return 0;
}

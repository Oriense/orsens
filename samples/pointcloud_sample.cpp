#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

#include "orsens.h"

Orsens orsens;
Mat disp, depth, color, cloud;

float scale = 1.0;
float angx=0, angy=M_PI/2;
float angstep=0.05;

const String cloud_window_name = "point cloud";

bool saveCloud(string cloud_filename = "test.ply", bool comma_decimal=false)
{
    ofstream cloud_file( cloud_filename.c_str() );

    if ( !cloud_file )
    {
        cerr << "Error opening output file: " << cloud_filename << "!" << endl;
        exit( 1 );
    }

    const int pointNum    = cloud.cols*cloud.rows;

    cloud_file << "ply" << endl;
    cloud_file << "format ascii 1.0" << endl;
    cloud_file << "element vertex " << pointNum << endl;
    cloud_file << "property float x" << endl;
    cloud_file << "property float y" << endl;
    cloud_file << "property float z" << endl;
    cloud_file << "property uchar red" << endl;
    cloud_file << "property uchar green" << endl;
    cloud_file << "property uchar blue" << endl;
    cloud_file << "end_header" << endl;

    for (int y=0; y<cloud.rows; y++)
        for (int x=0; x<cloud.cols; x++)
        {
            for (int i=0; i<3; i++)
            {
                float f = cloud.at<cv::Vec3f>(y,x)[i];

                if (!comma_decimal)
                {
                    cloud_file << f << " ";
                }
                else //for those dumb viewers which needs , as floating point separator (i.e. meshlab for ubuntu)
                {
                    ostringstream ss;
                    ss << f;
                    string s(ss.str());

                    size_t n = s.find(".");

                    if (n!=string::npos)
                    {
                        string s1 = s.substr(0, n);
                        string s2 = s.substr(n+1);
                        s = s1+","+s2;
                    }

                    cloud_file << s << " ";

                }

            }

            for (int i=0; i<3; i++)
            {
                uint8_t u = color.at<Vec3b>(y,x)[2-i];

                cloud_file << static_cast<int>(u) << " ";
            }

            cloud_file << endl;
        }
}

void rotateCamera()
{
    static double angle = 0.;
    static double radius = 3.;
    double x = radius*sin(angx);
    double y = radius*cos(angy);
    double z = radius*(1-cos(angx)) - radius/2;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(x,y,z,0,0,radius/2,0,1,0);
    glScalef(scale, -scale, scale);
}

void on_opengl(void* param)
{
    rotateCamera();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPointSize(1.0);
    glBegin(GL_POINTS);

    for (int y=0; y<cloud.rows; y++)
        for (int x=0; x<cloud.cols; x++)
        {
            Point3f pt = cloud.at<Vec3f>(y,x);
            glColor3f(color.at<Vec3b>(y,x)[2]/255.0, color.at<Vec3b>(y,x)[1]/255.0, color.at<Vec3b>(y,x)[0]/255.0);
            glVertex3f(pt.x/scale, pt.y/scale, pt.z/scale);
        }

    glEnd();

}

int main( int argc, char **argv )
{
    if (!orsens.start(Orsens::CAPTURE_DEPTH_LEFT))
    {
        printf("unable to start\n");
        return -1;
    }

    namedWindow(cloud_window_name,CV_WINDOW_OPENGL|CV_WINDOW_NORMAL);
    resizeWindow(cloud_window_name, 1024, 1024);
    setOpenGlDrawCallback(cloud_window_name,on_opengl,0);

    glClearColor(0,0,0,0);
    glClearDepth(1.0f);

    int width = 640;
    int height = 480;

    // Camera setup
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, width /(GLdouble) height, 0.1, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,0,0,0,0,1,0,1,0);

    while (true)
    {
        orsens.grabSensorData();

        cloud = orsens.getPointCloud();
        color = orsens.getLeft();

        imshow("left", color);
        imshow("depth", orsens.getDepth8());

        updateWindow(cloud_window_name);

        char c = (char)waitKey(1000/orsens.getRate());

        switch (c)
        {
        case 81:
            angx-=angstep;
            break;

        case 82:
            angy+=angstep;
            break;

        case 83:
            angx+=angstep;
            break;

        case 84:
            angy-=angstep;
            break;

        case 's':
            time_t rawtime;
            struct tm * timeinfo;

            time ( &rawtime );
            timeinfo = localtime ( &rawtime );
            char filename[256];
            strftime(filename, sizeof(filename), "%Y%m%d-%H%M%S.ply", timeinfo);
            printf("saving point cloud\n");
            saveCloud(filename, true);
            printf("saved to %s\n", filename);
            break;
        }

        if (c==27)
            break;

    }

    orsens.stop();

    return 0;
}

#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
//#include <GL/glut.h>

#include "orsens.h"

const String color_window_name = "color";
const String disp_window_name = "disparity";
const String depth_window_name = "depth";
const String cloud_window_name = "3d";

Orsens orsens;
Mat disp, depth, color;

float angx=0, angy=0;
float angstep=5;

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

void on_opengl(void* param)
{
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glTranslated(-0.5, -0.5, -0.0);

    glRotatef( angx, 1, 0, 0 );
    glRotatef( angy, 0, 1, 0 );
    glRotatef( 0, -1, 0, 1 );

    glPointSize(1);
    glBegin(GL_POINTS);

    /* for (int y=0; y<depth.rows; y++)
         for (int x=0; x<depth.cols; x++)
         {
           //  glColor3f( 1.0f, 1.0f, 1.0f );
          //   printf("d=%d ", depth.at<uint16_t>(y,x));
          float k=depth.cols;
          // float k=1.0;
            //
             glColor3f(color.at<Vec3b>(y,x)[2]/255.0, color.at<Vec3b>(y,x)[1]/255.0, color.at<Vec3b>(y,x)[0]/255.0);
          //   glVertex3f(x/k, y/k, disp.at<uint8_t>(y,x)/k);
             glVertex3f(x/k, (depth.rows-y)/k, 2*disp.at<uint8_t>(y,x)/k);
         }
    */

    //printf("\n");

    glEnd();

    // glutSwapBuffers();
}

int main( int argc, char **argv )
{
    if (!orsens.start(Orsens::CAPTURE_DEPTH_LEFT))
    {
        printf("unable to start\n");
        return -1;
    }

    namedWindow(depth_window_name);
    setMouseCallback(depth_window_name, onMouse, NULL);
    namedWindow(disp_window_name);
    setMouseCallback(disp_window_name, onMouse, NULL);
    namedWindow(color_window_name);
    setMouseCallback(color_window_name, onMouse, NULL);

    //  namedWindow(cloud_window_name,CV_WINDOW_OPENGL|CV_WINDOW_NORMAL);
    // resizeWindow(cloud_window_name, 1280, 480);
//   setOpenGlDrawCallback(cloud_window_name,on_opengl,0);

    while (true)
    {
        // just get the data we need
        orsens.grabSensorData(); //camera images and pose

        imshow(color_window_name, orsens.getLeft());
        imshow(disp_window_name, orsens.getDispColored());
        imshow(depth_window_name, orsens.getDepth8());

        //  updateWindow(cloud_window_name);

        char c = (char)waitKey(1000/orsens.getRate());

        if (c==27)
            break;

    }

    orsens.stop();

    return 0;
}

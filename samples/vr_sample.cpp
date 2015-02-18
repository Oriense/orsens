#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
//#include <GL/glut.h>

#include "orsens.h"

Orsens orsens;
Mat left, right;

int main( int argc, char **argv )
{
    if (!orsens.start(Orsens::CAPTURE_LEFT_RIGHT))
    {
        printf("unable to start\n");
        return -1;
    }

    while (true)
    {
        // just get the data we need
        orsens.grabSensorData();

        Mat left = orsens.getLeft();
        Mat right = orsens.getRight();

        imshow("left", left);
        imshow("right", right);
       // imshow("anaglyph", img3d);

        char c = (char)waitKey(1000/orsens.getRate());

        if (c==27)
            break;

    }

    orsens.stop();

    return 0;
}

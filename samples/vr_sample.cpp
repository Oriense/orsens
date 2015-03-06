#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

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

	 namedWindow("anaglyph", WINDOW_NORMAL);

    while (true)
    {
        // just get the data we need
        orsens.grabSensorData();

        Mat left = orsens.getLeft();
        Mat right = orsens.getRight();
        Mat anaglyph = left.clone();
        anaglyph = Scalar::all(0);

        Mat l_channels[3];
        Mat r_channels[3];

        split(left, l_channels);
        split(right, r_channels);

        vector<Mat> channels;

        channels.push_back(r_channels[0]);
        channels.push_back(r_channels[2]);
        channels.push_back(l_channels[1]);

        merge(channels, anaglyph);

        imshow("left", left);
        imshow("right", right);
        imshow("anaglyph", anaglyph);

        char c = (char)waitKey(1000/orsens.getRate());

        if (c==27)
            break;

    }

    orsens.stop();

    return 0;
}

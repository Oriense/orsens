#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include "orsens.h"

using namespace std;

Orsens orsens;
Mat left, right;

int main( int argc, char **argv )
{
    int color_width=1280;

    for( int i = 1; i < argc; i++ )
    {
        if( !strcmp( argv[i], "--help" ) || !strcmp( argv[i], "-h" ) )
        {
            cout << "-l      low resolution (if not enough usb bandwidth)" << endl;
            exit(0);
        }
        else if( !strcmp( argv[i], "-l" ) )
        {
            color_width = 640;
        }
    }

    if (!orsens.start(Orsens::CAPTURE_LEFT_RIGHT, "", color_width))
    {
        printf("unable to start\n");
        return -1;
    }

    namedWindow("anaglyph", WINDOW_NORMAL);
    resizeWindow("anaglyph", 1280, 960);

    while (true)
    {
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

        switch (c)
        {
        case 's':
            time_t rawtime;
            struct tm * timeinfo;

            time ( &rawtime );
            timeinfo = localtime ( &rawtime );
            char filename[256];
            strftime(filename, sizeof(filename), "anaglyph-%Y%m%d-%H%M%S.png", timeinfo);
            imwrite(filename, anaglyph);
            printf("saved to %s\n", filename);
            break;
        }

    }

    orsens.stop();

    return 0;
}

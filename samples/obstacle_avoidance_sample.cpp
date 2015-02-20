#include <iostream>

#include "orsens.h"

Orsens orsens;

int main()
{
    if (!orsens.start(Orsens::CAPTURE_DEPTH_ONLY))
    {
        printf("unable to start\n");
        return -1;
    }

    printf("camera's min and max distance: %d %d\n", orsens.getMinDistance(), orsens.getMaxDistance());

    while (true)
    {
        orsens.grabSensorData();

        Mat disp = orsens.getDisp();
        uint16_t dist = orsens.getNearestDistance();
        printf("dist: %d\n", dist);

        imshow("depth", disp);

        char c = waitKey(1000/orsens.getRate());

        if (c==27)
            break;
    }

    orsens.stop();

    return 0;
}

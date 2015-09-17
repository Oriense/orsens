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

        orsens.removeFloor();
        Mat disp = orsens.getDisp(true);
        ScenePoint pt = orsens.getSceneInfo(false).nearest_point;

        printf("%d %d: %4.2f %4.2f %4.2f\n", pt.pt_image.x, pt.pt_image.y, pt.pt_world.x, pt.pt_world.y, pt.pt_world.z);

        circle(disp, pt.pt_image, 5, Scalar(255,255,255));

        char s[255];
        sprintf(s, "%4.2f", pt.pt_world.z);
        putText(disp, s, Point(pt.pt_image.x-5, pt.pt_image.y-5), FONT_HERSHEY_TRIPLEX, 1.0, Scalar(255,255,255));

        imshow("depth", disp);

        char c = waitKey(1000/orsens.getRate());

        if (c==27)
            break;
    }

    orsens.stop();

    return 0;
}

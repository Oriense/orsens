#include <iostream>

#include "orsens.h"

Orsens orsens;

int main()
{
    if (!orsens.start(Orsens::CAPTURE_DEPTH_LEFT))
    {
        printf("unable to start\n");
        return -1;
    }

    while (true)
    {
        // just get the data we need
        orsens.grabSensorData();

        Mat segmentation_mask = orsens.getSegmentationMask(); //segment floor

        imshow("depth", orsens.getDispColored());
        imshow("segmentation mask", segmentation_mask);

        bitwise_not ( segmentation_mask, segmentation_mask );
        Mat segmented;
        orsens.getLeft().copyTo(segmented, segmentation_mask);

        imshow("segmented", segmented);

        char c = waitKey(1000/orsens.getRate());

        if (c==27)
            break;
    }

    orsens.stop();

    return 0;
}

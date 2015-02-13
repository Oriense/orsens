#include <iostream>

#include "orsens.h"

const String color_window_name = "color";
const String depth_window_name = "depth";

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
        orsens.grabSensorData(); //camera images and pose
        std::vector<Human> humans = orsens.getHumans(); //people in the scene

        // some visualization
        Mat color = orsens.getLeft();

        for( size_t i = 0; i < humans.size(); i++ )
        {
            printf("human %d: %4.2f %4.2f\n", i, humans[i].dist/1000.0, humans[i].angle);

            uint8_t disparity = orsens.dist2disp(humans[i].dist);
            Scalar dist_color = Scalar::all(disparity);

            ellipse( color, humans[i].centre, Size( humans[i].bounding_rect.width*0.5, humans[i].bounding_rect.height*0.5), 0, 0, 360, dist_color, 4, 8, 0 );

            char s[255];
            sprintf(s, "%3.2fm,%3.f ang", humans[i].dist/1000.0, humans[i].angle);
            putText(color, s, Point(humans[i].bounding_rect.x+5, humans[i].centre.y), FONT_HERSHEY_TRIPLEX, 1.0, dist_color);
        }

        imshow(color_window_name, color);
        imshow(depth_window_name, orsens.getDispColored());

        ///TODO get rate from lib
        char c = waitKey(1000/orsens.getRate());

        if (c==27)
            break;
    }

    orsens.stop();

    return 0;
}

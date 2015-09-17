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

    bool detect_gender = true;
    bool detect_age = true;

    Scalar draw_color = Scalar(0, 255, 0);

    Human human;

    while (true)
    {
        orsens.grabSensorData();
        Mat color = orsens.getLeft();

        if (orsens.getNearestHumanBiometrics(human))
        {
            rectangle( color, human.face_rect, draw_color, 3, 8, 0);

            circle( color, human.left_eye_pos, 2, Scalar(255,255,255), 3, 8, 0 );
            circle( color, human.right_eye_pos, 2, Scalar(255,255,255), 3, 8, 0 );

            putText(color, human.gender, Point(human.face_rect.x+5, human.face_rect.y+25), FONT_HERSHEY_TRIPLEX, 1.0, draw_color);

            char str[255];

            if (detect_age)
            {
                int k = human.age/10;
                sprintf(str, "age:%d-%d", k*10, (k+1)*10);
                putText(color, str, Point(human.face_rect.x+5, human.face_rect.y+50), FONT_HERSHEY_TRIPLEX, 0.7, draw_color);
            }

            sprintf(str, "%3.2fm", human.dist/1000.0);
            putText(color, str, Point(human.face_rect.x+5, human.face_rect.y+85), FONT_HERSHEY_TRIPLEX, 1.0, draw_color);

        }

        imshow(color_window_name, color);

        char c = waitKey(1000/orsens.getRate());

        if (c==27)
            break;
    }

    orsens.stop();

    return 0;
}

#include <iostream>

#include "Core.h"
#include "TrtEngine.h"
#include "Yolo5Engine.h"

using namespace std;

void PrintHelp()
{
    cout << "SkiingRobot 0.2" << endl;
    cout << "Usage: SkiingRobot <engine_file>" << endl;
    cout << "    engine_file: path to model TensorRT engine file" <<endl;
    cout << endl;
}

int PrintBadArguments()
{
    cout << "Bad arguments!" << endl;
    PrintHelp();
    return 1;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        return PrintBadArguments();
    }

    Yolo5Engine engine = Yolo5Engine(argv[1], 224, 224);
    Mat img = imread("G:/skiing/dataset/val/images/vid4_18.jpg");
    auto result = engine.DoInfer(img, 0.3);

    for (DetectedObject obj : result)
    {
        rectangle(img, Point((float)obj.bbox.xMin, (float)obj.bbox.yMin),
                  Point((float)obj.bbox.xMax, (float)obj.bbox.yMax),
                  Scalar(0, 255, 0));
    }

    imshow("Debug", img);
    waitKey();
}

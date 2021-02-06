#include <iostream>
#include <filesystem>

#include "Core.h"
#include "TrtEngine.h"
#include "Yolo5Engine.h"

using namespace std;
using namespace std::filesystem;

void PrintHelp()
{
    cout << "SkiingRobot 0.2" << endl;
    cout << "Usage: SkiingRobot <engine_file> <input>" << endl;
    cout << "    engine_file: path to model TensorRT engine file" <<endl;
    cout << "    input: path to input file or directory" <<endl;
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
    if (argc != 3)
    {
        return PrintBadArguments();
    }

    Yolo5Engine engine = Yolo5Engine(argv[1], 224, 224);

    if (is_directory(argv[2]))
    {
        for (const auto& entry : directory_iterator(argv[2]))
        {
            auto start = std::chrono::system_clock::now();
            string path = entry.path().string();
            Mat img = imread(path);
            auto result = engine.DoInfer(img, 0.3);
            auto end = std::chrono::system_clock::now();
            cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms : " << path << endl;
        }
    }
    else if (is_regular_file(argv[2]))
    {
        VideoCapture capture(argv[2]);
        Mat frame;
        while (capture.read(frame))
        {
            auto result = engine.DoInfer(frame, 0.3);

            for (DetectedObject obj : result)
            {
                rectangle(frame, Point((float)obj.bbox.xMin, (float)obj.bbox.yMin),
                          Point((float)obj.bbox.xMax, (float)obj.bbox.yMax),
                          Scalar(0, 255, 0));
            }

            imshow("Demo Result", frame);
        }
        waitKey();
    }
}

#include <iostream>
#include <filesystem>

#include "Core.h"
#include "TrtEngine.h"
#include "Yolo5Engine.h"

#ifndef WIN32
#include "Serial.hpp"
#endif

using namespace std;
using namespace std::filesystem;

void PrintHelp()
{
    cout << "SkiingRobot 0.2" << endl;
    cout << "Usage: SkiingRobot <engine_file> <flag>" << endl;
    cout << "    engine_file: path to model TensorRT engine file" <<endl;
    cout << "    flag: 1=save run video" << endl;
    cout << endl;
}

int PrintBadArguments()
{
    cout << "Bad arguments!" << endl;
    PrintHelp();
    return 1;
}

std::string get_current_time_and_date()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%H-%M-%S");
    return ss.str();
}

struct TargetInfo
{
    int x;
    int y;
};

TargetInfo onDetected(vector<DetectedObject> objs, Mat frame)
{

}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        return PrintBadArguments();
    }

    Yolo5Engine engine = Yolo5Engine(argv[1], 224, 224);
    VideoCapture capture(0);
    capture.set(CAP_PROP_FRAME_WIDTH, 1280);
    capture.set(CAP_PROP_FRAME_HEIGHT, 720);

    bool flag_save_video = false;
    VideoWriter writer;
    if (argc >= 3 && strcmp("1", argv[2]) == 0)
    {
        flag_save_video = true;
        std::stringstream filename;
        filename << "run_" << get_current_time_and_date() << ".mp4";
        writer = VideoWriter(filename.str(), VideoWriter::fourcc('M', 'P', '4', 'V'), 30, Size(1280, 720));
    }

#ifndef WIN32
    if (argc >= 4)
    {
        if (serialInit(argv[3]) == -1)
        {
            cout << "Serial init failed! << endl;
        }
    }
    else
    {
        if (serialInit((char*)"/dev/ttyUSB0") == -1)
        {
            cout << "Serial init failed! << endl;
        }
    }
#endif

    Mat frame;
    while (capture.read(frame))
    {
        auto start = std::chrono::system_clock::now();
        auto result = engine.DoInfer(frame, 0.3);
        auto end = std::chrono::system_clock::now();
        cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << endl;

        auto info = onDetected(result, frame);
#ifndef WIN32
        // send target info via serial port
        sendYawAngleSpeed(0);
#endif

        for (DetectedObject obj : result)
        {
            rectangle(frame, Point((float)obj.bbox.xMin, (float)obj.bbox.yMin),
                      Point((float)obj.bbox.xMax, (float)obj.bbox.yMax),
                      Scalar(0, 255, 0));
        }

        if (flag_save_video)
        {
            writer.write(frame);
        }
#ifdef WIN32
        imshow("Test", frame);
        if ((waitKey(1) & 0xFF) == 'q')
        {
            break;
        }
#endif
    }

    if (flag_save_video)
    {
        writer.release();
    }
}

#include <iostream>
#include <filesystem>

#include "Core.h"
#include "TrtEngine.h"
#include "Yolo5Engine.h"

#ifndef WIN32
#include <iomanip>
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

int y_sum_origin;
int color_change;
int color; // 0-blue; 1-red
int yaw_speed; //-128~-1 turn left; 0~127 turn right

struct TargetInfo
{
    int x;
    int y;
};

TargetInfo onDetected(vector<Yolo::Detection> objs, Mat frame)
{
    TargetInfo info = { };
    if (color == 0)
    {
        drawMarker(frame, Point(30, 30), (255, 0, 0), MARKER_SQUARE, 10, 10);
    }
    else
    {
        drawMarker(frame, Point(30, 30), (255, 0, 0), MARKER_SQUARE, 10, 10);
    }

    return info;
}

cv::Rect get_rect(cv::Mat& img, float bbox[4]) {
    int l, r, t, b;
    float r_w = Yolo::INPUT_W / (img.cols * 1.0);
    float r_h = Yolo::INPUT_H / (img.rows * 1.0);
    if (r_h > r_w) {
        l = bbox[0] - bbox[2] / 2.f;
        r = bbox[0] + bbox[2] / 2.f;
        t = bbox[1] - bbox[3] / 2.f - (Yolo::INPUT_H - r_w * img.rows) / 2;
        b = bbox[1] + bbox[3] / 2.f - (Yolo::INPUT_H - r_w * img.rows) / 2;
        l = l / r_w;
        r = r / r_w;
        t = t / r_w;
        b = b / r_w;
    } else {
        l = bbox[0] - bbox[2] / 2.f - (Yolo::INPUT_W - r_h * img.cols) / 2;
        r = bbox[0] + bbox[2] / 2.f - (Yolo::INPUT_W - r_h * img.cols) / 2;
        t = bbox[1] - bbox[3] / 2.f;
        b = bbox[1] + bbox[3] / 2.f;
        l = l / r_h;
        r = r / r_h;
        t = t / r_h;
        b = b / r_h;
    }
    return cv::Rect(l, t, r - l, b - t);
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
            cout << "Serial init failed!" << endl;
        }
    }
    else
    {
        if (serialInit((char*)"/dev/ttyUSB0") == -1)
        {
            cout << "Serial init failed!" << endl;
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

        for (size_t j = 0; j < result.size(); j++) {
            cv::Rect r = get_rect(frame, result[j].bbox);
            cv::rectangle(frame, r, cv::Scalar(0x27, 0xC1, 0x36), 2);
            cv::putText(frame, std::to_string((int)result[j].class_id), cv::Point(r.x, r.y - 1), cv::FONT_HERSHEY_PLAIN, 1.2, cv::Scalar(0xFF, 0xFF, 0xFF), 2);
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

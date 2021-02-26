#include <iostream>
#include <sstream>
#include <filesystem>

#include "Core.h"
#include "TrtEngine.h"
#include "Yolo5Engine.h"

#ifndef WIN32
#include <iomanip>
#include "Serial.hpp"
#endif

//#define VERBOSE_OUTPUT

using namespace std;
using namespace std::filesystem;

constexpr float conf_w = 0.1;
constexpr float area_w = 0.8;
constexpr float lowest_y_w = 0.1;
constexpr int gap_y_thres = 250;
constexpr int color_change_thres = 3;

Scalar color_map[2] = {
        Scalar(255, 0, 0),
        Scalar(0, 0, 255)
};

int y_sum_origin;
int color_change;
int color; // 0-blue; 1-red
int yaw_speed; //-128~-1 turn left; 0~127 turn right
unsigned char yaw_speed_tx;

struct TargetInfo
{
    int x;
    int y;
    bool flag;
    Rect b1r;
    Rect b2r;
};

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
//void drawMask(Mat frame, int )

TargetInfo findBBoxes(vector<Yolo::Detection> bboxes, int color, int y, int& y_sum, Mat frame) {
    Rect selected_boxes[2];
    float largest_score = 0;
    bool flag = false;
    TargetInfo info = { };
    info.flag = false;

    for (int i = 0; i < bboxes.size(); i++)
    {
        Yolo::Detection* b1 = &bboxes[i];
        for (int j = 0; j < bboxes.size(); j++)
        {
            Yolo::Detection* b2 = &bboxes[j];

            if (b1 != b2 && b1->class_id == b2->class_id)
            {
                Rect b1r = get_rect(frame, b1->bbox);
                Rect b2r = get_rect(frame, b2->bbox);

                if ((int)b1->class_id == color || y / 2 > b1r.height + b2r.height)
                {
                    float area = b1r.width * b1r.height + b2r.width * b2r.height;
                    float conf = b1->conf + b2->conf;
                    float gap_y = abs(b1r.height - b2r.height);
                    float lowest_y = -(abs((b1r.y - b1r.height / 2) - (b2r.y - b2r.height / 2)));

                    if (gap_y > gap_y_thres)
                    {
                        continue;
                    }

                    float score = conf_w * conf + area_w * area + lowest_y_w * lowest_y;
                    if (largest_score < score)
                    {
                        selected_boxes[0] = b1r;
                        selected_boxes[1] = b2r;
                        largest_score = score;
                        flag = true;
                        info.flag = true;
                        info.b1r = b1r;
                        info.b2r = b2r;
                    }
                }
            }
        }
    }

    if (flag)
    {
        info.x = (selected_boxes[0].x + selected_boxes[0].width / 2 + selected_boxes[1].x + selected_boxes[1].width / 2) / 2;
        info.y = (selected_boxes[0].y + + selected_boxes[0].height / 2 + selected_boxes[1].y + selected_boxes[1].height / 2) / 2;
        y_sum = selected_boxes[0].height + selected_boxes[1].height;
    }
    else
    {
        y_sum = 0;
    }
    return info;
}

TargetInfo onDetected(vector<Yolo::Detection> objs, Mat frame)
{
    TargetInfo result = { };
    int y_sum;
    drawMarker(frame, Point(30, 30), color_map[color], MARKER_SQUARE, 10, 10);
    result = findBBoxes(objs, color, y_sum_origin, y_sum, frame);

    int center_x = frame.cols / 2;
    yaw_speed = (center_x - result.x) * 0.2;
    if(yaw_speed > 127) yaw_speed = 127;
    if(yaw_speed < -128)yaw_speed = -128;

    if(yaw_speed < 0){
        cv::arrowedLine(frame, Point(frame.cols - 50, 30), Point(frame.cols - 90, 30), Scalar(0,0,255), 3, 0.5);
    } else{
        cv::arrowedLine(frame, Point(frame.cols - 50, 30), Point(frame.cols - 10, 30), Scalar(0,0,255), 3, 0.5);
    }
    stringstream stream;
    stream << "y_sum_origin:" << y_sum_origin;
    cv::putText(frame, stream.str(), Point(9, 75), cv::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 150, 255), 2);
    stream.str("");
    stream << "y_sum       :" << y_sum;
    cv::putText(frame, stream.str(), Point(9, 100), cv::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 150, 255), 2);
    stream.str("");
    stream << "yaw_speed   :" << yaw_speed;
    cv::putText(frame, stream.str(), Point(9, 125), cv::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 150, 255), 2);

    yaw_speed_tx = yaw_speed + 128;

    if(objs.size() > 2){
        if(y_sum != 0){
            if(y_sum > y_sum_origin){
                y_sum_origin = y_sum;
                color_change = 0;
            } else if(y_sum <= y_sum_origin / 2){
                color_change++;
                if(color_change == color_change_thres){
                    if(color == 0){
                        color = 1;
                    } else{
                        color = 0;
                    }
                    color_change = 0;
                    y_sum_origin = y_sum;
                }
            }
            return result;
        }
    }

    return result;
}

void PrintHelp()
{
    cout << "SkiingRobot 1.0" << endl;
    cout << "Usage: SkiingRobot <engine_file> <video_flag> <video_save_dir> <serial_port>" << endl;
    cout << "    engine_file: path to model TensorRT engine file" <<endl;
    cout << "    video_flag: 0=do not save video; 1=save run video; 2=save raw video" << endl;
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

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        return PrintBadArguments();
    }

    Yolo5Engine engine = Yolo5Engine(argv[1], 224, 224);
    VideoCapture capture(0);
    capture.set(CAP_PROP_FRAME_WIDTH, 1280);
    capture.set(CAP_PROP_FRAME_HEIGHT, 720);

    int video_flag = 0;
    VideoWriter writer;
    if (argc >= 3)
    {
        if (strcmp("1", argv[2]) == 0)
        {
            video_flag = 1;
        }
        else if (strcmp("2", argv[2]) == 0)
        {
            video_flag = 2;
        }

        if (video_flag != 0)
        {
            std::stringstream filename;
            filename << argv[3] << "/run_" << get_current_time_and_date() << ".mp4";
            writer = VideoWriter(filename.str(), VideoWriter::fourcc('M', 'P', '4', 'V'), 30, Size(1280, 720));
        }
    }

#ifndef WIN32
    if (argc >= 5)
    {
        if (serialInit(argv[4]) == -1)
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
#ifdef VERBOSE_OUTPUT
        cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << endl;
#endif

        auto info = onDetected(result, frame);
        auto end = std::chrono::system_clock::now();

#ifndef WIN32
        // send target info via serial port
        sendYawAngleSpeed(yaw_speed_tx);
#endif

        if (video_flag != 0)
        {
            for (size_t j = 0; j < result.size(); j++)
            {
                cv::Rect r = get_rect(frame, result[j].bbox);

                if (video_flag == 1)
                {
                    cv::rectangle(frame, r, color_map[(int)result[j].class_id], 2);
                    auto c1 = Point(r.x, r.y);
                    stringstream stream;
                    stream << fixed << setprecision(2) << result[j].conf;
                    string label = stream.str();
                    auto t_size = cv::getTextSize(label, cv::FONT_HERSHEY_PLAIN, 1.2, 2, nullptr);
                    auto c2 = Point(c1.x + t_size.width, c1.y - t_size.height - 3);
                    cv::rectangle(frame, c1, c2, color_map[(int)result[j].class_id], -1, LINE_AA);
                    cv::putText(frame, label, cv::Point(r.x, r.y - 1), cv::FONT_HERSHEY_PLAIN, 1.2, Scalar(255, 255, 255), 2);
                }
            }

            if (video_flag == 1)
            {
                if (info.flag)
                {
                    cv::rectangle(frame, Point(info.b1r.x, info.b1r.y), Point(info.b1r.x + info.b1r.width, info.b1r.y + info.b1r.height), Scalar(0, 255, 0), 2);
                    cv::rectangle(frame, Point(info.b2r.x, info.b2r.y), Point(info.b2r.x + info.b2r.width, info.b2r.y + info.b2r.height), Scalar(0, 255, 0), 2);
                }
            }
            stringstream ss;
            ss << "fps           : " << 1000 / std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            cv::putText(frame, ss.str(), Point(9, 150), cv::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 150, 255), 2);
            cv::drawMarker(frame, Point(info.x, info.y), Scalar(0, 255, 0), cv::MARKER_CROSS, 25, 2);
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

    if (video_flag != 0)
    {
        writer.release();
    }
}

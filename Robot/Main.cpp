#include <iostream>
#include <filesystem>
#include <queue>
#include <concurrent_queue.h>

#include "Core.h"
#include "TrtEngine.h"
#include "Yolo5Engine.h"

using namespace std;
using namespace std::filesystem;
using namespace concurrency;

void PrintHelp()
{
    cout << "SkiingRobot 0.3" << endl;
    cout << "Usage: SkiingRobot <engine_file> <input>" << endl;
    cout << "    engine_file: path to model TensorRT engine file" <<endl;
    cout << "    camera device: device name" <<endl;
    cout << endl;
}

int PrintBadArguments()
{
    cout << "Bad arguments!" << endl;
    PrintHelp();
    return 1;
}

void enqueue(VideoCapture* capture, Yolo5Engine* engine, concurrent_queue<Mat>* frameQueue)
{
    Mat frame;
    while (capture->read(frame))
    {
        auto start = std::chrono::system_clock::now();
        engine->EnqueueInfer(frame);
        frameQueue->push(frame);
        auto end = std::chrono::system_clock::now();
        cout << "Enqueue: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << endl;
    }
}

void dequeue(Yolo5Engine* engine, concurrent_queue<Mat>* frameQueue)
{
    while (true)
    {
        Mat frame;

        if (!frameQueue->try_pop(frame))
        {
            continue;
        }

        auto start = std::chrono::system_clock::now();

        auto result = engine->DequeueInfer(0.3f, frame.cols, frame.rows);
        for (DetectedObject obj : result)
        {
            rectangle(frame, Point((float)obj.bbox.xMin, (float)obj.bbox.yMin),
                      Point((float)obj.bbox.xMax, (float)obj.bbox.yMax),
                      Scalar(0, 255, 0));
        }



//        Mat display;
//        resize(frame, display, Size(1280, 720));
        imshow("Test", frame);

        auto end = std::chrono::system_clock::now();
        cout << "Dequeue: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << endl;

        if ((waitKey(1) & 0xFF) == 'q')
        {
            break;
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        return PrintBadArguments();
    }

    Yolo5Engine engine = Yolo5Engine(argv[1], 224, 224);
    VideoCapture capture(R"(G:\skiing\SkiingRobot\Control_Simulator\test_video\vid0.mp4)");
    concurrent_queue<Mat> frame_queue;

    thread t_en(enqueue, &capture, &engine, &frame_queue);
    thread t_de(dequeue, &engine, &frame_queue);

    t_de.join();
}

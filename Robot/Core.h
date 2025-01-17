//
// Created by hao on 2020/12/17.
//

#ifndef SKIINGROBOT_CORE_H
#define SKIINGROBOT_CORE_H

#include <NvInfer.h>
#include <string>
#include <fstream>
#include <opencv2/core.hpp>

inline std::string classes_dict[] = { "blue", "red" };

struct BBoxCoordinate {
    int xMin;
    int yMin;
    int xMax;
    int yMax;
};

struct DetectedObject {
    uint8_t classId;
    BBoxCoordinate bbox;
    float confidence;
};

inline bool fileExist (const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

void DrawRect(cv::Mat& img, DetectedObject obj);

#endif //SKIINGROBOT_CORE_H

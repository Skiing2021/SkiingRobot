//
// Created by Hao Su on 2021/1/24.
//

#ifndef SKIINGROBOT_YOLO5ENGINE_H
#define SKIINGROBOT_YOLO5ENGINE_H

#include "Core.h"
#include "TrtEngine.h"

class Yolo5Engine : public TrtEngine {
public:
    float NMS_Threshold;

    Yolo5Engine(const string &modelPath, int modelWidth, int modelHeight, float nmsThreshold = DEFAULT_NMS_THRESHOLD);
    vector<DetectedObject> DoInfer(const Mat& image, float confidenceThreshold) override;

protected:
    void PreProcess(const Mat &img) override;
    vector<DetectedObject> PostProcess(float confidenceThreshold, int originWidth, int originHeight) override;

private:
    static constexpr float DEFAULT_NMS_THRESHOLD = 0.5f;
};


#endif //SKIINGROBOT_YOLO5ENGINE_H

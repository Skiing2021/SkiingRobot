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

    /// Resize image to a 32-pixel-multiple rectangle https://github.com/ultralytics/yolov3/issues/232
    Mat LetterBoxResize(const Mat &img);

protected:
    vector<Mat> PreProcess(const Mat &img) override;
    vector<DetectedObject> PostProcess(vector<float*> outputs, float confidenceThreshold, int originWidth, int originHeight) override;

private:
    static constexpr float DEFAULT_NMS_THRESHOLD = 0.5f;
};


#endif //SKIINGROBOT_YOLO5ENGINE_H

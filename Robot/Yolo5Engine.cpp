//
// Created by Hao Su on 2021/1/24.
//

#include <map>
#include <vector>
#include "Yolo5Engine.h"
#include "tensorrtx/yolov5/yololayer.h" // register yolo plugin

using namespace cv;

Yolo5Engine::Yolo5Engine(const string &modelPath, int modelWidth, int modelHeight, float nmsThreshold)
    : TrtEngine(modelPath, modelWidth, modelHeight)
{
    this->NMS_Threshold = nmsThreshold;
}

void Yolo5Engine::PreProcess(const Mat &img) {
    // letter box resize
    int w, h, x, y;
    float r_w = _modelWidth / (img.cols*1.0);
    float r_h = _modelHeight / (img.rows*1.0);
    if (r_h > r_w) {
        w = _modelWidth;
        h = r_w * img.rows;
        x = 0;
        y = (_modelHeight - h) / 2;
    } else {
        w = r_h * img.cols;
        h = _modelHeight;
        x = (_modelWidth - w) / 2;
        y = 0;
    }
    cv::Mat re(h, w, CV_8UC3);
    cv::resize(img, re, re.size(), 0, 0, cv::INTER_LINEAR);
    cv::Mat out(_modelHeight, _modelWidth, CV_8UC3, cv::Scalar(128, 128, 128));
    re.copyTo(out(cv::Rect(x, y, re.cols, re.rows)));

    int i = 0;
    for (int row = 0; row < _modelHeight; ++row) {
        uchar* uc_pixel = out.data + row * out.step;
        for (int col = 0; col < _modelWidth; ++col) {
            hostBuffers[0][i] = (float)uc_pixel[2] / 255.0;
            hostBuffers[0][i + _modelHeight * _modelWidth] = (float)uc_pixel[1] / 255.0;
            hostBuffers[0][i + 2 * _modelHeight * _modelWidth] = (float)uc_pixel[0] / 255.0;
            uc_pixel += 3;
            ++i;
        }
    }
}

float iou(BBoxCoordinate *bbox, BBoxCoordinate *bbox_next) {
    int interBox[] = {
            max(bbox->xMin, bbox_next->xMin),
            min(bbox->xMax, bbox_next->xMax),
            max(bbox->yMin, bbox_next->yMin),
            min(bbox->yMax, bbox_next->yMax)
    };

    if (interBox[2] > interBox[3] || interBox[0] > interBox[1])
    {
        return 0.0f;
    }

    int interBoxS = (interBox[1] - interBox[0]) * (interBox[3] - interBox[2]);
    int area_bbox = (bbox->xMax - bbox->xMin) * (bbox->yMax - bbox->yMin);
    int area_bbox_next = (bbox_next->xMax - bbox_next->xMin) * (bbox_next->yMax - bbox_next->yMin);
    return (float)interBoxS / (float)(area_bbox + area_bbox_next - interBoxS);
}

bool cmp(const DetectedObject& a, const DetectedObject& b) {
    return a.confidence > b.confidence;
}

vector<DetectedObject>
Yolo5Engine::PostProcess(float confidenceThreshold, int originWidth, int originHeight) {
    vector<DetectedObject> objectList { };

    for (int i = 1; i <= buffersSize[1]; i += 6)
    {
        float confidence = hostBuffers[1][i + 4];
        if (confidence >= confidenceThreshold)
        {
            DetectedObject object = { };
            object.classId = hostBuffers[1][i + 5];
            object.confidence = confidence;

            BBoxCoordinate b = { };
            float centerX = hostBuffers[1][i];
            float centerY = hostBuffers[1][i + 1];
            float w = hostBuffers[1][i + 2];
            float h = hostBuffers[1][i + 3];
            w /= 2;
            h /= 2;

            b.xMin = (int)round(centerX - w);
            b.yMin = (int)round(centerY - h);
            b.xMax = (int)round(centerX + w);
            b.yMax = (int)round(centerY + h);
            object.bbox = b;

            objectList.push_back(object);
        }
    }

    // Non-maximum Suppression
    vector<DetectedObject> ret;
    sort(objectList.begin(), objectList.end(), cmp);
    for (int i = 0; i < objectList.size(); i++)
    {
        auto& obj = objectList[i];
        ret.push_back(obj);
        for (int j = i + 1; j < objectList.size(); j++)
        {
            if (iou(&obj.bbox, &objectList[j].bbox) > this->NMS_Threshold)
            {
                objectList.erase(objectList.begin() + j);
                j--;
            }
        }
    }

    // rescale letter-boxed bbox coordinates -> coordinates with respect to the origin image
    float r_w = (float)_modelWidth / (float)originWidth;
    float r_h = (float)_modelHeight / (float)originHeight;
    for (DetectedObject& obj : ret)
    {
        if (r_h > r_w)
        {
            obj.bbox.xMin = (int)((float)obj.bbox.xMin / r_w);
            obj.bbox.xMax = (int)((float)obj.bbox.xMax / r_w);
            obj.bbox.yMin = (int)(((float)obj.bbox.yMin - ((float)_modelHeight - r_w * (float)originHeight) / 2) / r_w);
            obj.bbox.yMax = (int)(((float)obj.bbox.yMax - ((float)_modelHeight - r_w * (float)originHeight) / 2) / r_w);
        }
        else
        {
            obj.bbox.xMin = (int)(((float)obj.bbox.xMin - ((float)_modelWidth - r_w * (float)originWidth) / 2) / r_w);
            obj.bbox.xMax = (int)(((float)obj.bbox.xMax - ((float)_modelWidth - r_w * (float)originWidth) / 2) / r_w);
            obj.bbox.yMin = (int)((float)obj.bbox.yMin / r_h);
            obj.bbox.yMax = (int)((float)obj.bbox.yMax/ r_h);
        }

    }

    return ret;
}

vector<DetectedObject> Yolo5Engine::DoInfer(const Mat &image, float confidenceThreshold) {
    PreProcess(image);

    cudaMemcpyAsync(deviceBuffers[0], hostBuffers[0], buffersSizeInBytes[0], cudaMemcpyHostToDevice, _stream);
    _context->enqueue(1, reinterpret_cast<void **>(deviceBuffers.data()), _stream, nullptr);
    cudaMemcpyAsync(hostBuffers[1], deviceBuffers[1], buffersSizeInBytes[1], cudaMemcpyDeviceToHost, _stream);
    cudaStreamSynchronize(_stream);

    return PostProcess(confidenceThreshold, image.cols, image.rows);
}

void Yolo5Engine::EnqueueInfer(const Mat &image)
{
    PreProcess(image);
    cudaMemcpyAsync(deviceBuffers[0], hostBuffers[0], buffersSizeInBytes[0], cudaMemcpyHostToDevice, _stream);
    _context->enqueue(1, reinterpret_cast<void **>(deviceBuffers.data()), _stream, nullptr);
}

vector<DetectedObject> Yolo5Engine::DequeueInfer(float confidenceThreshold, int originWidth, int originHeight)
{
    cudaMemcpyAsync(hostBuffers[1], deviceBuffers[1], buffersSizeInBytes[1], cudaMemcpyDeviceToHost, _stream);
    cudaStreamSynchronize(_stream);

    return PostProcess(confidenceThreshold, originWidth, originHeight);
}

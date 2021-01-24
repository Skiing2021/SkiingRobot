//
// Created by Hao Su on 2021/1/24.
//

#include <map>
#include <vector>
#include "Yolo5Engine.h"
#include "tensorrtx/yolov5/yololayer.h" // register yolo plugin

using namespace cv;

Yolo5Engine::Yolo5Engine(const string &modelPath, int modelWidth, int modelHeight) : TrtEngine(modelPath, modelWidth,
                                                                                               modelHeight) {

}

vector<Mat> Yolo5Engine::PreProcess(const Mat &img) {
    Mat imgResized;
    Mat imgFloat;

    imgResized = LetterBoxResize(img);
    imgResized.convertTo(imgFloat, CV_32FC3, 1.0, 0); //uint8 -> float, divide by 255

    Mat c;
    Mat h;
    Mat w;
    extractChannel(imgFloat, c, 0);
    extractChannel(imgFloat, h, 1);
    extractChannel(imgFloat, w, 2);

    vector<Mat> ret;
    ret.push_back(c);
    ret.push_back(h);
    ret.push_back(w);

    return ret;
}

bool cmp(const DetectedObject& a, const DetectedObject& b) {
    return a.confidence > b.confidence;
}

vector<DetectedObject>
Yolo5Engine::PostProcess(vector<float *> outputs, float confidenceThreshold, int originWidth, int originHeight) {
    std::vector<DetectedObject> objectList { };
    for (int i = 0; i < 20; i ++)
    {
        cout << outputs[0][i] << endl;
    }

    for (int i = 1; i <= buffersSize[1]; i += 6)
    {
        float confidence = outputs[0][i + 4];
        if (confidence >= confidenceThreshold)
        {
            DetectedObject object = { };
            object.classId = outputs[0][i + 5];
            object.confidence = confidence;

            BBoxCoordinate b = { };
            float centerX = outputs[0][i];
            float centerY = outputs[0][i + 1];
            float w = outputs[0][i + 2];
            float h = outputs[0][i + 3];
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

    // nms
    std::sort(objectList.begin(), objectList.end(), cmp);
    //TODO: NMS & IOU

    return objectList;
}

// Implementation adapted from:
// https://github.com/ultralytics/yolov5/blob/d9212140b355b84e85a473be590720eb8221766c/utils/datasets.py#L780-L810
Mat Yolo5Engine::LetterBoxResize(const Mat &img) {
    // Scale ratio (new /old)
    float ratio = min((float)this->_modelHeight / (float)img.size[0], (float)this->_modelWidth / (float)img.size[1]);
    // only scale down
    ratio = min(ratio, 1.0f);

    // Compute padding
    int unpad_w = int(round((float)img.size[1] * ratio));
    int unpad_h = int(round((float)img.size[0] * ratio));
    float dw = (float)this->_modelWidth - (float)unpad_w;
    float dh = (float)this->_modelHeight - (float)unpad_h;

    // Divide padding into 2 sides
    dw /= 2;
    dh /= 2;

    Mat resized;
    if (img.size[0] != unpad_h || img.size[1] != unpad_w)
    {
        resize(img, resized, Size(unpad_w, unpad_h));
    }
    else
    {
        resized = img;
    }

    int top = int(round(dh - 0.1));
    int bottom = int(round(dh + 0.1));
    int left = int(round(dw - 0.1));
    int right = int(round(dw + 0.1));

    Mat ret;
    copyMakeBorder(resized, ret, top, bottom, left, right, BORDER_CONSTANT, Scalar(114, 114, 114));

    return ret;
}

vector<DetectedObject> Yolo5Engine::DoInfer(const Mat &image, float confidenceThreshold) {
    auto img = PreProcess(image);

    size_t per = buffersSizeBytes[0] / 3;

    cudaMemcpyAsync(deviceBuffers[0], img[0].data, per, cudaMemcpyHostToDevice, _stream);
    cudaMemcpyAsync(deviceBuffers[0] + per, img[1].data, per, cudaMemcpyHostToDevice, _stream);
    cudaMemcpyAsync(deviceBuffers[0] + per * 2, img[2].data, per, cudaMemcpyHostToDevice, _stream);

    _context->enqueue(1, reinterpret_cast<void **>(deviceBuffers.data()), _stream, nullptr);

    cudaMemcpyAsync(hostOutputBuffers[0], deviceBuffers[1], buffersSizeBytes[1], cudaMemcpyDeviceToHost, _stream);
//    cudaMemcpyAsync(hostOutputBuffers[1], deviceBuffers[2], buffersSizeBytes[2], cudaMemcpyDeviceToHost, _stream);
    cudaStreamSynchronize(_stream);

    img.clear();
    return PostProcess(hostOutputBuffers, confidenceThreshold, image.cols, image.rows);
}

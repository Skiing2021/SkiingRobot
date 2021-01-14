import cv2
import argparse

from sklearn.metrics._plot import det_curve

from yolo_detect import YoloDetector

def onDetected(bboxes):
    """
    Analyze bounding boxes and infer the target position
    Prototyping skiing robot control algorithm here
    :param bboxes: bounding boxes torch tensor;
        Example tensor:
            [tensor([
                    [142.25000,  98.31250, 148.25000, 107.93750,   0.50928,   0.00000],
                    [107.56250,  94.00000, 112.68750, 102.75000,   0.42310,   0.00000]
                ], device='cuda:0')]
        Example code:
            for i, det in enumerate(pred):  # detections per image
                if len(det):
                    for *xyxy, conf, cls in reversed(det):

    :return: a (x, y) tuple; indicates the expected target position in the given frame image;
    """

    # Just a demo to show how to use this simulator
    if (len(bboxes) >= 2):
        return ((bboxes[0].xmin + bboxes[1].xmin) / 2, (bboxes[0].ymin + bboxes[1].ymin) / 2)
    else:
        return (112, 112)

# Recommend test video
# https://www.youtube.com/watch?v=8GwpubtvueM
# https://www.youtube.com/watch?v=M4XbJqRO00s
# https://www.youtube.com/watch?v=JsJY9U74l7w
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--yolov5-root', type=str, default='./yolov5', help='Path to yolov5 root directory')
    parser.add_argument('--weights', nargs='+', type=str, default='yolov5s.pt', help='model.pt path(s)')
    parser.add_argument('--source', type=str, default='test_video/vid0.MOV', help='source')  # file/folder, 0 for webcam
    parser.add_argument('--img-size', type=int, default=224, help='inference size (pixels)')
    parser.add_argument('--conf-thres', type=float, default=0.25, help='object confidence threshold')
    parser.add_argument('--iou-thres', type=float, default=0.45, help='IOU threshold for NMS')
    parser.add_argument('--device', default='', help='cuda device, i.e. 0 or 0,1,2,3 or cpu')
    opt = parser.parse_args()
    print(opt)

    cap = cv2.VideoCapture("test_video/vid0.MOV")
    window_name = "Skiing Control Simulator    -    Press \"Q\" to exit"

    detector = YoloDetector(opt.weights, opt.device, opt.img_size, opt.conf_thres, opt.iou_thres)
    while True:
        ret, frame = cap.read()

        result = detector.Detect(frame)

        # Visualize detection results
        line_thickness = round(0.002 * (frame.shape[0] + frame.shape[1]) / 2) + 1
        font_thickness = max(line_thickness - 1, 1)
        for i, det in enumerate(result):  # detections per image
            if len(det):
                for *xyxy, conf, cls in reversed(det):
                    xmin, ymin, xmax, ymax = (int(xyxy[0]), int(xyxy[1]), int(xyxy[2]), int(xyxy[3]))
                    cls = int(cls)

                    # rescale to origin
                    c1 = (int(xmin * frame.shape[1] / detector.imgsz), int(ymin * frame.shape[0] / detector.imgsz))
                    c2 = (int(xmax * frame.shape[1] / detector.imgsz), int(ymax * frame.shape[0] / detector.imgsz))

                    cv2.rectangle(frame, c1, c2, detector.colors[cls],
                                  thickness=line_thickness, lineType=cv2.LINE_AA)
                    label = f'{detector.names[int(cls)]} {conf:.2f}'
                    t_size = cv2.getTextSize(label, 0, fontScale=line_thickness / 3, thickness=font_thickness)[0]
                    c2 = c1[0] + t_size[0], c1[1] - t_size[1] - 3
                    cv2.rectangle(frame, c1, c2, detector.colors[cls], -1, cv2.LINE_AA)  # filled
                    cv2.putText(frame, label, (c1[0], c1[1] - 2), 0, line_thickness / 3, [225, 255, 255], thickness=font_thickness,
                                lineType=cv2.LINE_AA)

        onDetected(result)

        display = cv2.resize(frame, (1280, 720))
        cv2.imshow(window_name, display)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break





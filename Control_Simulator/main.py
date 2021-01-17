import cv2
import argparse
from yolo_detect import YoloDetector
from auto_control import onDetected


class BBox:
    def __init__(self, xmin, ymin, xmax, ymax, conf, cls):
        """
        Initialize bounding box info object
        :param xmin: x-coordinate of the top-left corner
        :param ymin: y-coordinate of the top-left corner
        :param xmax: x-coordinate of the bottom-right corner
        :param ymax: y-coordinate of the top-left corner
        :param conf: confidence
        :param cls: class id
        """

        self.xmin = xmin
        self.ymin = ymin
        self.xmax = xmax
        self.ymax = ymax
        self.conf = conf
        self.cls = cls


# Recommended test video
# https://www.youtube.com/watch?v=8GwpubtvueM
# https://www.youtube.com/watch?v=M4XbJqRO00s
# https://www.youtube.com/watch?v=JsJY9U74l7w
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--weights', nargs='+', type=str, default='yolov5s.pt', help='model.pt path(s)')
    parser.add_argument('--source', type=str, default='test_video/vid0.MOV', help='source')  # file/folder, 0 for webcam
    parser.add_argument('--img-size', type=int, default=224, help='inference size (pixels)')
    parser.add_argument('--conf-thres', type=float, default=0.4, help='object confidence threshold')
    parser.add_argument('--iou-thres', type=float, default=0.45, help='IOU threshold for NMS')
    parser.add_argument('--device', default='', help='cuda device, i.e. 0 or 0,1,2,3 or cpu')
    opt = parser.parse_args()
    print(opt)

    cap = cv2.VideoCapture(opt.source)
    window_name = "Skiing Control Simulator    -    Press \"Q\" to exit"

    detector = YoloDetector(opt.weights, opt.device, opt.img_size, opt.conf_thres, opt.iou_thres)
    while True:
        ret, frame = cap.read()

        result = detector.Detect(frame)

        # Visualize detection results
        line_thickness = round(0.002 * (frame.shape[0] + frame.shape[1]) / 2) + 1
        font_thickness = max(line_thickness - 1, 1)

        bbox = []
        for i, det in enumerate(result):  # detections per image
            if len(det):
                for *xyxy, conf, cls in reversed(det):
                    xmin, ymin, xmax, ymax = (int(xyxy[0]), int(xyxy[1]), int(xyxy[2]), int(xyxy[3]))
                    cls = int(cls)

                    c1 = (xmin, ymin)
                    c2 = (xmax, ymax)

                    cv2.rectangle(frame, c1, c2, detector.colors[cls],
                                  thickness=line_thickness, lineType=cv2.LINE_AA)
                    label = f'{detector.names[int(cls)]} {conf:.2f}'
                    t_size = cv2.getTextSize(label, 0, fontScale=line_thickness / 3, thickness=font_thickness)[0]
                    c2 = c1[0] + t_size[0], c1[1] - t_size[1] - 3
                    cv2.rectangle(frame, c1, c2, detector.colors[cls], -1, cv2.LINE_AA)  # filled
                    cv2.putText(frame, label, (c1[0], c1[1] - 2), 0, line_thickness / 3, [225, 255, 255],
                                thickness=font_thickness,
                                lineType=cv2.LINE_AA)
                    bbox.append(BBox(xmin, ymin, xmax, ymax, conf, cls))

        target = onDetected(bbox, frame)
        c3 = (int(target[0]), int(target[1]))
        cv2.drawMarker(frame, c3, [0, 255, 0], markerType=cv2.MARKER_CROSS, thickness=line_thickness, markerSize=75)

        display = cv2.resize(frame, (1280, 720))
        cv2.imshow(window_name, display)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

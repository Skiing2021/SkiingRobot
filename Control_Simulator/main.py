import cv2
import sys
from yolo_detect import YoloDetector

def onDetected(bboxes):
    """
    Analyze bounding boxes and infer the target position
    Prototyping skiing robot control algorithm here
    :param bboxes: bounding boxes array; each bounding box element is a (xmin, ymin, xmax, ymax) tuple
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
    cap = cv2.VideoCapture("test_video/vid0.MOV")
    window_name = "Skiing Control Simulator    -    Press \"Q\" to exit"

    detector = YoloDetector()

    while True:
        ret, frame = cap.read()
        # rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        # rgb = cv2.resize(rgb, 224, 224)

        # TODO: yolov5 inference 
        # result = model(rgb)
        # onDetected(result)

        display = cv2.resize(frame, (1280, 720))
        cv2.imshow(window_name, display)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break





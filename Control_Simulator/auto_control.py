import cv2

def drawMask(img, xMin, yMin, xMax, yMax):
    alphaReserve = 0.8
    BChannel = 0
    GChannel = 255
    RChannel = 0

    img[yMin:yMax, xMin:xMax, 0] = img[yMin:yMax, xMin:xMax, 0] * alphaReserve + BChannel * (1 - alphaReserve)
    img[yMin:yMax, xMin:xMax, 1] = img[yMin:yMax, xMin:xMax, 1] * alphaReserve + GChannel * (1 - alphaReserve)
    img[yMin:yMax, xMin:xMax, 2] = img[yMin:yMax, xMin:xMax, 2] * alphaReserve + RChannel * (1 - alphaReserve)

y_sum_origin = 0
color_change = 0
color = 0

conf_w = 0.1
area_w = 0.8
lowestY_w = 0.1
gap_y_thres = 250

yaw_speed = 0

def findBiggestBBox(bboxes, color, y, cv2_img=None):
    b_BBox = [[None, None],  # largest two blue bboxes
              [None, None]]  # largest two red bboxes
    b_score = [[0, 0], [0, 0]]

    selected_boxes = [0, 1]
    largest_score = 0
    for b1 in bboxes:
        for b2 in bboxes:
            if b1 != b2 and b1.cls == b2.cls and (b1.cls == color or y/2 > abs(abs(b1.ymax - b1.ymin) + abs(b2.ymax - b2.ymin))):
                area_1 = abs((b1.xmax - b1.xmin) * (b1.ymax - b1.ymin))
                area_2 = abs((b2.xmax - b2.xmin) * (b2.ymax - b2.ymin))
                conf_1 = b1.conf
                conf_2 = b2.conf
                area = area_1 + area_2
                conf = conf_1 + conf_2
                gap_y = abs(abs(b1.ymax - b1.ymin) - abs(b2.ymax - b2.ymin))
                lowest_y = -(abs(b1.ymin - b2.ymin))
                if gap_y > gap_y_thres:
                    continue
                score = conf_w * conf + area_w * area + lowestY_w * lowest_y
                if largest_score < score:
                    selected_boxes[0] = b1
                    selected_boxes[1] = b2
                    largest_score = score

    if selected_boxes != [0, 1]:

        if (selected_boxes[0]):
            drawMask(cv2_img, selected_boxes[0].xmin, selected_boxes[0].ymin, selected_boxes[0].xmax,
                     selected_boxes[0].ymax)
        if (selected_boxes[1]):
            drawMask(cv2_img, selected_boxes[1].xmin, selected_boxes[1].ymin, selected_boxes[1].xmax,
                     selected_boxes[1].ymax)

        if selected_boxes[0] and selected_boxes[1]:
            target_x = (((selected_boxes[0].xmin + selected_boxes[0].xmax) / 2) + (
                    (selected_boxes[1].xmin + selected_boxes[1].xmax) / 2)) / 2
            target_y = (((selected_boxes[0].ymin + selected_boxes[0].ymax) / 2) + (
                    (selected_boxes[1].ymin + selected_boxes[1].ymax) / 2)) / 2
            # print(b_score)
            # print((gates[0].ymin, gates[0].ymax), (gates[1].ymin, gates[1].ymax))
            y_sum = abs(selected_boxes[0].ymax - selected_boxes[0].ymin) + abs(
                selected_boxes[1].ymax - selected_boxes[1].ymin)
            # print((target_x, target_y), selected_boxes[0].cls, y_sum)
            return (target_x, target_y), y_sum

    else:
        return (0, 0), 0

# def findHighestBBox(bboxes, cv2_img=None):
#     red_dict = dict()
#     blue_dict = dict()
#
#     for b in bboxes:
#         if b.cls == 0:
#             blue_dict[b.ymax - b.ymin] = b
#         else:
#             red_dict[b.ymax - b.ymin] = b
#
#     red_dict = dict(sorted(red_dict.items(), key=lambda item: item[0], reverse=True))
#     blue_dict = dict(sorted(blue_dict.items(), key=lambda item: item[0], reverse=True))
#
#     if len(red_dict) < 2 and len(blue_dict) < 2:
#         return None
#     elif len(red_dict) >= 2:


def onDetected(bboxes, cv2_img=None):
    """
    Analyze bounding boxes and infer the target position
    Prototyping skiing robot control algorithm here
    :param bboxes: bounding boxes array;
        each bounding box element is a BBox object
    :return: a (x, y) tuple; indicates the expected target position in the given frame image;
    """

    global y_sum_origin, color_change, color, yaw_speed
    if color == 0:
        cv2.drawMarker(cv2_img, (30, 30), markerType=cv2.MARKER_SQUARE, markerSize=10, color=[255, 0, 0], thickness=10)
    else:
        cv2.drawMarker(cv2_img, (30, 30), markerType=cv2.MARKER_SQUARE, markerSize=10, color=[0, 0, 255], thickness=10)
    result, y_sum = findBiggestBBox(bboxes, color,y_sum_origin, cv2_img)

    if result and (result != (0, 0)):
        # Test
        center_x = cv2_img.shape[1] / 2
        if result[0] < center_x:
            yaw_speed = -128
        else:
            yaw_speed = 127

        if yaw_speed < 0:
            cv2.arrowedLine(cv2_img, (cv2_img.shape[1] - 50, 30), (cv2_img.shape[1] - 90, 30), color=(0,0,255), thickness=3, tipLength=0.5)
        else:
            cv2.arrowedLine(cv2_img, (cv2_img.shape[1] - 50, 30), (cv2_img.shape[1] - 10, 30), color=(0,0,255), thickness=3, tipLength=0.5)

    cv2.putText(cv2_img, "y_sum_origin: " + str(y_sum_origin), (9, 75), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 150, 255), 2)
    cv2.putText(cv2_img, "y_sum       : " + str(y_sum), (9, 100), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 150, 255), 2)
    cv2.putText(cv2_img, "yaw_speed: " + str(yaw_speed), (cv2_img.shape[1] - 150, 75), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 150, 255), 2)
    if (len(bboxes) >= 2):
        if y_sum != 0:
            if y_sum > y_sum_origin:
                y_sum_origin = y_sum
                color_change = 0
            elif y_sum <= y_sum_origin / 2:
                color_change = color_change + 1
                if color_change == 3:
                    print('color has changed')
                    if color == 0:
                        color = 1
                    else:
                        color = 0
                    color_change = 0
                    y_sum_origin = y_sum
            if result and result != (0, 0):
                # print(y_sum)
                return result

    return 0, 0

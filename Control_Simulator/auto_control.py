
def drawMask(img, xMin, yMin, xMax, yMax):
    alphaReserve = 0.8
    BChannel = 0
    GChannel = 255
    RChannel = 0

    img[yMin:yMax, xMin:xMax, 0] = img[yMin:yMax, xMin:xMax, 0] * alphaReserve + BChannel * (1 - alphaReserve)
    img[yMin:yMax, xMin:xMax, 1] = img[yMin:yMax, xMin:xMax, 1] * alphaReserve + GChannel * (1 - alphaReserve)
    img[yMin:yMax, xMin:xMax, 2] = img[yMin:yMax, xMin:xMax, 2] * alphaReserve + RChannel * (1 - alphaReserve)


def findBiggestBBox(bboxes, cv2_img=None):
    b_BBox = [[None, None],  # largest two blue bboxes
              [None, None]]  # largest two red bboxes
    b_area = [[0, 0], [0, 0]]

    for b in bboxes:
        area = abs((b.xmax - b.xmin) * (b.ymax - b.ymin))
        if not (b_BBox[b.cls][0]):
            b_BBox[b.cls][0] = b
            b_area[b.cls][0] = area
        else:
            if area > b_area[b.cls][0]:
                b_BBox[b.cls][1] = b_BBox[b.cls][0]
                b_BBox[b.cls][0] = b
                b_area[b.cls][1] = b_area[b.cls][0]
                b_area[b.cls][1] = area
            elif not (b_BBox[b.cls][1]):
                b_BBox[b.cls][1] = b
                b_area[b.cls][1] = area
            elif area > b_area[b.cls][1]:
                b_BBox[b.cls][1] = b
                b_area[b.cls][1] = area

    len_blue = 0
    if b_BBox[0][0]:
        len_blue = len_blue + 1
    if b_BBox[0][1]:
        len_blue = len_blue + 1

    len_red = 0
    if b_BBox[1][0]:
        len_red = len_red + 1
    if b_BBox[1][1]:
        len_red = len_red + 1

    gates = None
    if len_blue == 2 and len_red == 2:
        if b_area[0][0] + b_area[0][1] > b_area[1][0] + b_area[1][1]:
            gates = b_BBox[0] # blue
        else:
            gates = b_BBox[1] # red
    elif len_blue < 2:
        gates = b_BBox[1] # red
    elif len_red < 2:
        gates = b_BBox[0] # blue

    if gates:
        if (gates[0]):
            drawMask(cv2_img, gates[0].xmin, gates[0].ymin, gates[0].xmax, gates[0].ymax)
        if (gates[1]):
            drawMask(cv2_img, gates[1].xmin, gates[1].ymin, gates[1].xmax, gates[1].ymax)

        if gates[0] and gates[1]:
            target_x = (((gates[0].xmin + gates[0].xmax) / 2) + ((gates[1].xmin + gates[1].xmax) / 2)) / 2
            target_y = (((gates[0].ymin + gates[0].ymax) / 2) + ((gates[1].ymin + gates[1].ymax) / 2)) / 2
            return target_x, target_y
    else:
        return None

def onDetected(bboxes, cv2_img=None):
    """
    Analyze bounding boxes and infer the target position
    Prototyping skiing robot control algorithm here
    :param bboxes: bounding boxes array;
        each bounding box element is a BBox object
    :return: a (x, y) tuple; indicates the expected target position in the given frame image;
    """

    # Just a demo to show how to use this simulator
    if (len(bboxes) >= 2):
        result = findBiggestBBox(bboxes, cv2_img)
        if (result):
            return result

    return 0, 0

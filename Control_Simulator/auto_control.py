from main import BBox

def onDetected(bboxes):
    """
    Analyze bounding boxes and infer the target position
    Prototyping skiing robot control algorithm here
    :param bboxes: bounding boxes array;
        each bounding box element is a BBox object
    :return: a (x, y) tuple; indicates the expected target position in the given frame image;
    """

    # Just a demo to show how to use this simulator
    if (len(bboxes) >= 2):
        return ((bboxes[0].xmin + bboxes[1].xmin) / 2, (bboxes[0].ymin + bboxes[1].ymin) / 2)
    else:
        return (112, 112)